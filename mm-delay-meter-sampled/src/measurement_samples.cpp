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
unsigned long num_runs;
int idle_mic_val;

int16_t samples[NUM_SAMPLES];

void measurementSamplesSetup()
{
	// Initialize measurement series
	measured_delay_flag = 0;
	light_recieved_at_sensor_flag = 0;
	sound_recieved_at_mic_flag = 0;
    pinMode(lightSensorPin, INPUT);
    pinMode(microphonePin, INPUT);
    measurementSamplesInitialize();
    num_runs = 0;

    // Initialize sampling interval
    startTimer3();
    pauseTimer3();
    timer3ClearSamplingFlag();
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

void measurementSamplesInitialize()
{
	for (uint8_t i = 0; i < NUM_SAMPLES; i++){
		samples[i] = 0;
	}
	current_index = 0;
	max_sample = 0;
	max_sample_index = 0;
}

double measurementSamplesGetDelayMs()
{
	return deltaMicros/1000.0;
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
int16_t prev_max;
int16_t prev_prev_max;
bool edge_detected;

void measurementSamplesRisingEdgeDetection(int mode)
{
	/*TEST*/
	digitalWrite(testFreqPin, digitalRead(testFreqPin)^1);

	switch (mode)
	{
		case VIDEO_MODE:
			measurementSamplesRisingEdgeDetectionVideo();
			break;
		case SOUND_MODE:
			measurementSamplesRisingEdgeDetectionSound();
			break;
	}
}

void measurementSamplesRisingEdgeDetectionVideo()
{
	static int16_t acc_pos_slopes;
	// Get timestamp as early as possible
	delta = readTimer1();

	current_max = measurementSamplesMaxSmoothingFilter();  //analogRead(lightSensorPin); //?
	edge_detected = false;

	if (num_runs >= 3 && !light_recieved_at_sensor_flag){
		if (current_max - prev_max > 10){
			edge_detected = true;
		}
		else{
			if (current_max > prev_max){
				acc_pos_slopes++;
				//Serial.println(acc_pos_slopes);
			} 
			else acc_pos_slopes = 0;
			if (acc_pos_slopes >= 5){
				edge_detected = true;
			}
		}
		
    	if (edge_detected){
			deltaMicros = microsFromCounting((unsigned long)delta);
	    	measured_delay_flag = 1;
			light_recieved_at_sensor_flag = 1;
			acc_pos_slopes = 0;
		}
	}
	prev_max = current_max;
	num_runs++;
}

void setIdleMicVal(int val)
{
	idle_mic_val = val;
}

void measurementSamplesRisingEdgeDetectionSound()
{
	static int16_t acc_pos_slopes;
	// Get timestamp as early as possible
	delta = readTimer1();

	current_max = analogReadFast(microphonePin);
	edge_detected = false;

	if (!sound_recieved_at_mic_flag){
		if (current_max - idle_mic_val > 5){
			edge_detected = true;
		}
		else{
			if (current_max > idle_mic_val){
				acc_pos_slopes++;
				Serial.println(acc_pos_slopes);
			} 
			else acc_pos_slopes = 0;
			if (acc_pos_slopes >= 2){
				edge_detected = true;
			}
		}
		//edge_detected = (current_max > idle_mic_val); // Can not do this
		//if (edge_detected) Serial.println(current_max - idle_mic_val);
		// Otherwise, compare previous 3 values. Increased >10 and strictly increasing
		// if(!edge_detected){
  //     		edge_detected = (current_max - prev_prev_max > 10 && current_max >= prev_max && prev_max >= prev_prev_max); 
  //   	}
    	if (edge_detected){
			deltaMicros = microsFromCounting((unsigned long)delta);
	    	measured_delay_flag = 1;
			sound_recieved_at_mic_flag = 1;
		}
	}
	//num_runs++;
}



void measurementSamplesClearLightRecievedFlag()
{
	light_recieved_at_sensor_flag = 0;
}

void measurementSamplesClearSoundRecievedFlag()
{
	sound_recieved_at_mic_flag = 0;
}