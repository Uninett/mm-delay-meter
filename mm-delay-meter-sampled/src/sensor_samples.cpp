#include "sensor_samples.h"
#include "timer3.h"
#include "timer1.h"
#include "config.h"

bool light_recieved_at_sensor_flag;
bool sound_recieved_at_mic_flag;
bool measured_delay_flag;
int16_t idle_mic_val;
int16_t idle_ltv_val;

/* Maximum smoothing filter */
int16_t samples[NUM_SAMPLES];
uint8_t current_index;
uint16_t max_sample;
uint8_t max_sample_index;

/* Rising edge detection */
uint8_t i_m;
int16_t current_max;
unsigned long deltaMicros;
unsigned long deltaMicrosSaved[BUF_SIZE];

void sensorSamplesSetup(uint8_t mode)
/* Initialize status flags. Set pin directions for sensor pins.
 * Set initial mode (video). 
 * Specify the ADC clock frequency, which speeds up the conversion compared to the default. */
{
	i_m = 0;
	measured_delay_flag = 0;
	light_recieved_at_sensor_flag = 0;
	sound_recieved_at_mic_flag = 0;
    pinMode(lightSensorPin, INPUT);
    pinMode(microphonePin, INPUT);

    sensorSamplesSetMode(mode);
    sensorSamplesInitialize(mode);

    /* ADC speed - prescaler 16 */ 
    ADCSRA = (ADCSRA & B11111000) | 4; 
}

void sensorSamplesSetMode(uint8_t mode)
/* Initialize the sampling timer and get idle samples 
 * from the sensor corresponding to the current mode. */
{
	/* Initialize sampling interval */
    startTimer3(mode);
    pauseTimer3();
    timer3ClearSamplingFlag();
    
    resetSavedMeasurements();

	if (mode == VIDEO_MODE){
		/* Change ADC's Vref to default 5V */
		analogReference(DEFAULT);
		int dummyRead;
		for (int i = 0; i < 100; i++){
			dummyRead = analogRead(A0);
		}

    	/* Calibrate light sensor measurements */
    	Log.println(F("Set VIDEO mode"));
		int current_ltv = analogRead(lightSensorPin);
		int prev_ltv = current_ltv;
		int mean = 0;
		int ltv[NUM_SAMPLES] = {0};
		int idle_iterations = 0;
		while (idle_iterations < NUM_SAMPLES){
			current_ltv = analogRead(lightSensorPin);
			if (abs(current_ltv - prev_ltv) < 5){
				ltv[idle_iterations] = current_ltv;
				idle_iterations++;
			}
			else{
				ltv[NUM_SAMPLES] = {0};
				idle_iterations = 0;
			}
			prev_ltv = current_ltv;
		}

		for (auto &v: ltv){
			mean += v;
		}
		mean = mean/NUM_SAMPLES;    
		idle_ltv_val = mean;
    }

	if(mode == SOUND_MODE){
		/* Change ADC's Vref to internal 2.56V */
		analogReference(INTERNAL);
		int dummyRead;
		for (int i = 0; i < 100; i++){
			dummyRead = analogRead(A1);
		}

		/* Calibrate microphone measurements */
		Log.println(F("Set AUDIO mode"));
		int current_mic = analogRead(microphonePin);
		int prev_mic = current_mic;
		int mean = 0;
		int mic[NUM_SAMPLES] = {0};
		int idle_iterations = 0;
		while (idle_iterations < NUM_SAMPLES){
			current_mic = analogRead(microphonePin);
			if (abs(current_mic - prev_mic) < 5){
				mic[idle_iterations] = current_mic;
				idle_iterations++;
			}
			else{
				mic[NUM_SAMPLES] = {0};
				idle_iterations = 0;
			}
			prev_mic = current_mic;
		}

		for (auto &m: mic){
			mean += m;
		}
		mean = mean/NUM_SAMPLES;    
		idle_mic_val = mean;
	}
}

void sensorSamplesInitialize(uint8_t mode)
/* Initialize the list of samples from the current sensor to the same idle value */
{
	for (int i = 0; i < NUM_SAMPLES; i++){
		samples[i] = (mode == VIDEO_MODE) ? idle_ltv_val : idle_mic_val;
	}
	current_index = 0;
	max_sample = (mode == VIDEO_MODE) ? idle_ltv_val : idle_mic_val;
	max_sample_index = 0;
}

static int16_t sensorSamplesMaxSmoothingFilter()
/* Keeps track of a window of sensor samples and the current maximum sample within this window.
 * When the maximum from one sample to the next changes "enough", an edge is detected.
 * Makes the edge detection more robust. */
{
	int16_t raw_sample = analogRead(lightSensorPin);

	// Add new sample to saved samples
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


/* Rising edge detection */

static void sensorSamplesRisingEdgeDetectionVideo(bool &edge_detected, bool start_new_series)
/* Uses the max smoothing filter to detect when a sufficient increase in light intensity is 
 * measured. If the increase between two subsequent samples is large enough, or there has 
 * been a continuous increase over enough samples, there is an edge. */
{
	current_max = sensorSamplesMaxSmoothingFilter();
	edge_detected = false;

	static uint8_t acc_pos_slopes;
	static int16_t prev_max = current_max;
	if (start_new_series){
		acc_pos_slopes = 0;
		prev_max = current_max;
	}

	if (!light_recieved_at_sensor_flag){
		if (current_max - prev_max > 10) edge_detected = true;
		else{
			if (current_max > prev_max){
				acc_pos_slopes++;
			} 
			else acc_pos_slopes = 0;
			if (acc_pos_slopes >= 5) edge_detected = true;
		}
		
    	if (edge_detected) acc_pos_slopes = 0;
	}
	prev_max = current_max;
}

static void sensorSamplesRisingEdgeDetectionSound(bool &edge_detected, bool start_new_series)
/* If the current sample is higher than the idle value plus a small safety margin,
 * or if the samples have been above idle for some time, there is an egde.
 * The max smoothing filter will not work here because the sound waves oscillate with 
 * too high frequency. */
{
	static uint8_t num_pos_measures;
	if (start_new_series){
		num_pos_measures = 0;
	}
	current_max = analogRead(microphonePin);
	edge_detected = false;

	if (!sound_recieved_at_mic_flag){
		if (current_max - idle_mic_val > 5) edge_detected = true; 
		else{
			if (current_max > idle_mic_val+1){
				num_pos_measures++;
			} 
			else num_pos_measures = 0;
			if (num_pos_measures >= 2) edge_detected = true; 
		}

    	if (edge_detected) num_pos_measures = 0; 
	}
}

bool sensorSamplesRisingEdgeDetection(uint8_t mode, bool start_new_series)
/* When an edge is detected, the time since signal generation is recorded. 
 * Keeps track of how many measurements that have been made and blinks status LEDs accordingly.
 * Signals when the first edge in a series is detected and when a full series is completed. */
{
	// Get timestamp as early as possible
	unsigned short delta = readTimer1();
	bool edge_detected = false;

	switch (mode)
	{
		case VIDEO_MODE:
			sensorSamplesRisingEdgeDetectionVideo(edge_detected, start_new_series);
			break;
		case SOUND_MODE:
			sensorSamplesRisingEdgeDetectionSound(edge_detected, start_new_series);
			break;
	}

	if (edge_detected){
		deltaMicrosSaved[i_m++] = microsFromCounting((unsigned long)delta);
		if (mode == VIDEO_MODE) light_recieved_at_sensor_flag = 1;
		if (mode == SOUND_MODE) sound_recieved_at_mic_flag = 1;

		if (i_m > 0 && i_m <= BUF_PART_1){
			// Blink statusLed1
			digitalWrite(statusLedPin1, HIGH);
		}
		else if (i_m > BUF_PART_1 && i_m <= BUF_PART_2){
			// Blink statusLed2
			digitalWrite(statusLedPin1, HIGH);
			digitalWrite(soundModeIndicator, HIGH);
		}
		else if (i_m > BUF_PART_2 && i_m <= BUF_SIZE){
			// Blink statusLed3
			digitalWrite(statusLedPin1, HIGH);
			digitalWrite(soundModeIndicator, HIGH);
			digitalWrite(videoModeIndicator, HIGH);
		}
		if (i_m >= BUF_SIZE){
			i_m = 0;
    		measured_delay_flag = 1; // Save measurements in SD card
		}
		else if (i_m == 1){
			return true; // First edge detected
		}
	}
	return false;
}

void resetNumMeasurementsCompleted()
/* Reset the counter when a new measurement series can begin */
{
	i_m = 0;
}
uint8_t getNumMeasurementsCompleted()
/* Keep track of the number of measurements that have been made at a given moment */
{
	return i_m;
}

void setMeasuredCompleteFlag()
/* Flag set when a measurement series is stopped and measurements must be saved in SD card */
{
	measured_delay_flag = 1;
}
void clearMeasuredCompleteFlag()
/* Flag cleared after a measurement series is saved in SD card and a new one can begin */
{
	measured_delay_flag = 0;
}
bool checkMeasuredCompleteFlag()
/* Check the flag to see if any measurements must be saved in SD card */
{
	return measured_delay_flag;
}

void clearLightRecievedFlag()
{
	light_recieved_at_sensor_flag = 0;
}
void clearSoundRecievedFlag()
{
	sound_recieved_at_mic_flag = 0;
}

void resetSavedMeasurements()
/* Reset the delay measurements to get ready for a new series */
{
	for (uint8_t i = 0; i < BUF_SIZE; i++){
		deltaMicrosSaved[i] = 0;
	}
}
String getSavedMeasurement(uint8_t index)
/* Fetch a specific delay measurement */
{
	String sample = String(deltaMicrosSaved[index]/1000.0);
	return sample;
}