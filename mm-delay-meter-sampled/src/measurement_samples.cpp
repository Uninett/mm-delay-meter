/* Samples the light sensor every 200us (5kHz), and measures delay since light was sent to the LED.
 * Implements a maximum smoothing filter and rising edge detection for detecting 
 * a recieved light pulse. */
#include "measurement_samples.h"
#include "timer3.h"
#include "timer1.h"
#include "config.h"
#include "avdweb_analogReadFast.h"

bool light_recieved_at_sensor_flag;
bool sound_recieved_at_mic_flag;
bool measured_delay_flag;
unsigned short delta;
unsigned long deltaMicros;
int idle_mic_val;
int idle_ltv_val;
bool first_edge_detected;

int16_t samples[NUM_SAMPLES];

void measurementSamplesSetup(int mode)
{
	// Initialize measurement series
	measured_delay_flag = 0;
	light_recieved_at_sensor_flag = 0;
	sound_recieved_at_mic_flag = 0;
	first_edge_detected = false;
    pinMode(lightSensorPin, INPUT);
    pinMode(microphonePin, INPUT);
    if (mode == VIDEO_MODE){
    	Serial.println("Calibrating light-to-voltage sensor...");
		int current_ltv = analogRead(lightSensorPin);
		int prev_ltv = current_ltv;
		int mean = 0;
		int ltv[10] = {0};
		int idle_iterations = 0;
		while (idle_iterations < 10){
			current_ltv = analogRead(lightSensorPin);
			if (abs(current_ltv - prev_ltv) < 5){
				Serial.print(idle_iterations);
				Serial.print("\t");
				Serial.print(prev_ltv);
				Serial.print("\t");
				Serial.println(current_ltv);
				ltv[idle_iterations] = current_ltv;
				idle_iterations++;
			}
			else{
				Serial.println("Reset");
				ltv[10] = {0};
				idle_iterations = 0;
			}
			prev_ltv = current_ltv;
		}

		for (auto &v: ltv){
			mean += v;
		}
		mean = mean/10;    
		idle_ltv_val = mean;
		Serial.print("Mean l-t-v: ");
		Serial.println(mean);
    }

    // Initialize sampling interval
    startTimer3(mode);
    pauseTimer3();
    timer3ClearSamplingFlag();

    // Calibrate microphone measurements
	if(mode == SOUND_MODE){
		/* Change ADC's Vref to internal 2.56V */
		Serial.println("ADC setup");
		analogReference(INTERNAL);
		int dummyRead;
		for (int i = 0; i < 100; i++){
			dummyRead = analogRead(A1);
			Serial.println(dummyRead);
		}
		Serial.println("ADC setup commplete");


		Serial.println("Calibrating microphone...");
		int current_mic = analogRead(microphonePin);
		int prev_mic = current_mic;
		int mean = 0;
		int mic[10] = {0};
		int idle_iterations = 0;
		while (idle_iterations < 10){
			current_mic = analogRead(microphonePin);
			if (abs(current_mic - prev_mic) < 5){
				Serial.print(idle_iterations);
				Serial.print("\t");
				Serial.print(prev_mic);
				Serial.print("\t");
				Serial.println(current_mic);
				mic[idle_iterations] = current_mic;
				idle_iterations++;
			}
			else{
				Serial.println("Reset");
				mic[10] = {0};
				idle_iterations = 0;
			}
			prev_mic = current_mic;
		}

		for (auto &m: mic){
			mean += m;
		}
		mean = mean/10;    
		setIdleMicVal(mean);
		Serial.print("Mean: ");
		Serial.println(mean);
	}
    measurementSamplesInitialize(mode);

    /* ADC speed - prescaler 16 */ 
    ADCSRA = (ADCSRA & B11111000) | 4; 
}

bool measurementSamplesCheckMeasuredFlag()
{
	if (measured_delay_flag){ 
    	measured_delay_flag = 0;
    	return true;
 	}
 	else{
    	return false;
 	}
}


/* Maximum smoothing filter */
uint8_t current_index;
uint16_t max_sample;
uint8_t max_sample_index;

void measurementSamplesInitialize(int mode)
{
	for (int i = 0; i < NUM_SAMPLES; i++){
		samples[i] = (mode == VIDEO_MODE) ? idle_ltv_val : idle_mic_val;
	}
	current_index = 0;
	max_sample = 0;
	max_sample_index = 0;
}

int16_t measurementSamplesMaxSmoothingFilter()
{
	int16_t raw_sample = analogReadFast(lightSensorPin);

	// Add sample to saved samples
	samples[current_index] = raw_sample;

	// Check if new sample replaces old max in samples[]
	if (max_sample_index == current_index){
		// Find the new maximum of NUM_SAMPLES previous samples
		max_sample = 0;
		for (uint8_t i = 0; i < NUM_SAMPLES; i++){
		    if (samples[i] > max_sample){
		    	max_sample = samples[i];
		    	max_sample_index = i;
		    }
		}
	}

	// Check if new sample is new max
	else if (max_sample < samples[current_index]){
		max_sample = samples[current_index];
		max_sample_index = current_index;
	}

	current_index = (current_index < NUM_SAMPLES - 1) ? current_index + 1 : 0;
	return max_sample;
}


// Rising edge detection
int16_t current_max;
bool edge_detected;
unsigned long deltaMicrosSaved[BUF_SIZE];


bool measurementSamplesRisingEdgeDetection(int mode)
{
	static uint8_t i_m;
	// Get timestamp as early as possible
	delta = readTimer1();

	/*TEST*/
	//digitalWrite(testFreqPin, digitalRead(testFreqPin)^1);
	//Writing a logic one to PINxn toggles the value of PORTxn, digital pin 6 is PD7
	//PIND = 0x80;

	switch (mode)
	{
		case VIDEO_MODE:
			edge_detected = measurementSamplesRisingEdgeDetectionVideo();
			break;
		case SOUND_MODE:
			edge_detected = measurementSamplesRisingEdgeDetectionSound();
			break;
	}

	if (edge_detected){
		deltaMicrosSaved[i_m++] = microsFromCounting((unsigned long)delta);
		if (mode == VIDEO_MODE) light_recieved_at_sensor_flag = 1;
		if (mode == SOUND_MODE) sound_recieved_at_mic_flag = 1;

		Serial.print(i_m);
		Serial.print("\t");
		Serial.println(deltaMicrosSaved[i_m-1]);
		if (i_m >= BUF_SIZE){
			i_m = 0;
    		measured_delay_flag = 1; // Save measurements in SD card
		}
		else if (i_m == 1){
			return true; // First edge detected
		}
		// if (!first_edge_detected){
		// 	// Tell rest of program that the first successful measurements have started
		// 	first_edge_detected = true;
		// }
	}
	return false;
}

bool measurementSamplesRisingEdgeDetectionVideo()
{
	static int16_t acc_pos_slopes;

	current_max = measurementSamplesMaxSmoothingFilter();
	static int16_t prev_max = current_max;
	edge_detected = false;

	if (!light_recieved_at_sensor_flag){
		if (current_max - prev_max > 10) edge_detected = true;
		else{
			if (current_max > prev_max){
				acc_pos_slopes++;
				//Serial.println(acc_pos_slopes);
			} 
			else acc_pos_slopes = 0;
			if (acc_pos_slopes >= 5) edge_detected = true;
		}
		
    	if (edge_detected) acc_pos_slopes = 0;
	}
	prev_max = current_max;
	return edge_detected;
}

bool measurementSamplesRisingEdgeDetectionSound()
{
	static int16_t num_pos_measures;

	current_max = analogReadFast(microphonePin);
	edge_detected = false;

	if (!sound_recieved_at_mic_flag){
		if (current_max - idle_mic_val > 5) edge_detected = true; 
		else{
			if (current_max > idle_mic_val+1){
				num_pos_measures++;
				Serial.println(num_pos_measures);
			} 
			else num_pos_measures = 0;
			if (num_pos_measures >= 2) edge_detected = true; 
		}

    	if (edge_detected) num_pos_measures = 0; 
	}
	return edge_detected;
}

void setIdleMicVal(int val)
{
	idle_mic_val = val;
}

double measurementSamplesGetDelayMs()
{
	return deltaMicros/1000.0;
}

void measurementSamplesClearLightRecievedFlag()
{
	light_recieved_at_sensor_flag = 0;
}

void measurementSamplesClearSoundRecievedFlag()
{
	sound_recieved_at_mic_flag = 0;
}

unsigned long measurementSamplesGetSavedSample(uint8_t index)
{
	return deltaMicrosSaved[index];
}