/* Detects recieved light at sensor using hardware interrupt.
 * Measures delay of recieved light.
 * This timer should be reset in the function generating pulses */

#include "Arduino.h"
#include "measure_led.h"
#include "Timer3/Timer3.h"
#include "signal_generator_led.h"

volatile bool led_flag;
volatile unsigned short delta;
volatile unsigned long deltaMicros;
unsigned long num_runs;

unsigned short samples[NUM_SAMPLES];


void measureLEDSetup()
{
    // Timer 3 for measuring delay, 64us resolution
    startUltraSlowCountingTimer3();
    pauseTimer3();

    led_flag = 0;
    pinMode(lightSensorInterruptPin, OUTPUT);	// Can generate software interrupt
    digitalWrite(lightSensorInterruptPin, LOW);
    attachInterrupt(digitalPinToInterrupt(lightSensorInterruptPin), measureLEDDelay, RISING);

    measureLEDInitializeSamples();
    num_runs = 0;
}

/* Interrupt service routine for external interrupt at pin 3, triggers when light is recieved at sensor */
void measureLEDDelay()
{
    // Stop timer
    pauseTimer3();

    // Save delay measurement

    // readTimer1() returns a maximum value of 65535
    // That means the maximum possible delta one can measure with this
    // function (when in ultra slow counting mode) is 4194ms on 16 MHz boards,
    // and 2097ms on 8 MHz boards
    delta = readTimer3();

    // If you estimate deltaMicros could be > 65 ms, or 65535 us,
    // delta should be cast to unsigned long, and deltaMicros should be
    // created as an unsigned long variable
    deltaMicros = microsFromUltraSlowCounting((unsigned long)delta);

    led_flag = 1;
}

// debug only
// void printSensorValue(){
//     // Read analog value (for "debug")
//     unsigned short raw_sample = analogRead(lightSensorPin);
//     Serial.print("Light sensor pin: ");
//     Serial.println(raw_sample);
// }
// void printDelay(){
// 	Serial.print("Light delay: ");
// 	Serial.print(deltaMicros);
// 	Serial.println(" us");
// }
//

bool measureLEDCheckFlag()
{
	if (led_flag){ 
    	led_flag = 0;
    	return true;
 	}
 	else{
    	return false;
 	}
}

void measureLEDPrintToSerial()
{
	Serial.println(deltaMicros/1000.0);
}

double measureLEDGetDelayMs()
{
	return deltaMicros/1000.0;
}



/* Maximum smoothing filter */
void measureLEDInitializeSamples()
{
	for (uint8_t i = 0; i < NUM_SAMPLES; i++){
		samples[i] = 0;
	}
}

unsigned short measureLEDMaxSmoothingFilter()
{
	static uint8_t current_index;
	static unsigned short max_sample;
	static uint8_t max_sample_index;

	unsigned short raw_sample = analogRead(lightSensorPin);

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

	if (current_index < NUM_SAMPLES - 1){
		current_index++;
	} else{
		current_index = 0;
	}

	return max_sample;
}


// Rising edge detection
void measureLEDRisingEdgeDetection()
{
	
	int current_max = measureLEDMaxSmoothingFilter();
	static int prev_max = current_max;
	static int prev_prev_max = current_max;
	static int prev_prev_prev_max = current_max;

	// static int max_samples[NUM_MAX_SAMPLES] = {0};
	// static uint8_t current_index;

	bool edge_detected = false;

	if (num_runs >= 3 && !SGLEDCheckFlag()){
		edge_detected = (current_max - prev_max > 10);
		// Otherwise, compare previous 3 values. Increased >20 and strictly increasing
		if(!edge_detected){
			edge_detected = (current_max - prev_prev_max > 10 && current_max > prev_max && prev_max > prev_prev_max);
		}
		if (!edge_detected){
			edge_detected = (current_max - prev_prev_prev_max > 10 
							&& current_max > prev_max && prev_max > prev_prev_max && prev_prev_max > prev_prev_prev_max);
		}
		
		if (edge_detected){
			digitalWrite(lightSensorInterruptPin, HIGH);
			SGLEDSetFlag(1);	
		}
	}
	prev_prev_prev_max = prev_prev_max;
	prev_prev_max = prev_max;
	prev_max = current_max;


	// max_samples[current_index] = current_max;
	// if (num_runs >= NUM_MAX_SAMPLES && !SGLEDCheckFlag()){
	// 	uint8_t i = 0;
	// 	uint8_t j = NUM_MAX_SAMPLES - 1 - current_index;
	// 	while(i < NUM_MAX_SAMPLES){
	// 		if (i <= current_index){
	// 			max_samples[current_index - i]
	// 			// Check previous max samples, until current_i - i

	// 		}
	// 		else if (j > 0){
	// 			max_samples[current_index + j]
	// 			j--;
	// 		}
			

	// 		i++;
	// 	}

	// 	if (edge_detected){
	// 		digitalWrite(lightSensorInterruptPin, HIGH);
	// 		SGLEDSetFlag(1);	
	// 	}
	// }
		
	// if (current_index < NUM_MAX_SAMPLES - 1){
	// 	current_index++;
	// } else{
	// 	current_index = 0;
	// }
	
	num_runs++;
}

// bool measureLEDCompare(const int* max_samples, const uint8_t last_check_index, const uint8_t first_check_index)
// {
// 	for (uint8_t i = last_check_index-1; i >= first_check_index; i--){
// 		if (max_samples[i] <= max_samples[i-1]){
// 			// Not strictly increasing
// 			return false;
// 		}
// 		if (max_samples[last_check_index] - max_samples[i] > 10){
			
// 		}
// 	}
// }

