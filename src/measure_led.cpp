/* Detects recieved light at sensor using hardware interrupt.
 * Measures delay of recieved light.
 * This timer should be reset in the function generating pulses */

#include "Arduino.h"
#include "measure_led.h"
#include "signal_generator_led.h"
#include "timer1.h"
#include "config.h"

volatile bool measured_delay_flag;
volatile unsigned short delta;
volatile unsigned long deltaMicros;
volatile bool sample_flag;
unsigned long num_runs;

unsigned short samples[NUM_SAMPLES];


void measureLEDFromTimer1Setup()
{
    measured_delay_flag = 0;
    pinMode(lightSensorInterruptPin, OUTPUT);	// Can generate software interrupt
    pinMode(lightSensorPin, INPUT);
    digitalWrite(lightSensorInterruptPin, LOW);
    //attachInterrupt(digitalPinToInterrupt(lightSensorInterruptPin), measureLEDDelayFromTimer1, RISING);

    measureLEDInitializeSamples();
    num_runs = 0;
}

void measureLEDDelayFromTimer1()
{
	// Stop timer?
    //pauseTimer1();

    // Save delay measurement
    delta = readTimer1();
    deltaMicros = microsFromCounting((unsigned long)delta);

    measured_delay_flag = 1;
}



bool measureLEDCheckFlag()
{
	if (measured_delay_flag){ 
    	measured_delay_flag = 0;
    	return true;
 	}
 	else{
    	return false;
 	}
}

void measureLEDPrintToSerial()
{
	Serial.println(measureLEDGetDelayMs());
}

double measureLEDGetDelayMs()
{
	return deltaMicros/1000.0;
}



/* Maximum smoothing filter */
uint8_t current_index;
uint16_t max_sample;
uint8_t max_sample_index;

void measureLEDInitializeSamples()
{
	for (uint8_t i = 0; i < NUM_SAMPLES; i++){
		samples[i] = 0;
	}
	current_index = 0;
	max_sample = 0;
	max_sample_index = 0;
}


uint16_t measureLEDMaxSmoothingFilter()
{
	uint16_t raw_sample = analogRead(lightSensorPin);

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
void measureLEDRisingEdgeDetection()
{
	delta = readTimer1();

	current_max = measureLEDMaxSmoothingFilter();

	edge_detected = false;

	if (num_runs >= 3 && !SGLEDCheckFlag()){
		edge_detected = (current_max - prev_max > 20);
		// Otherwise, compare previous 3 values. Increased >20 and strictly increasing
		if(!edge_detected){
      		edge_detected = (current_max - prev_prev_max > 20 && current_max >= prev_max && prev_max >= prev_prev_max); 
    	}
		
		
	}
	prev_prev_max = prev_max;
	prev_max = current_max;

	if (edge_detected){
		// Generate interrupt signal that reads the stopwatch
		deltaMicros = microsFromCounting((unsigned long)delta);
    	measured_delay_flag = 1;
		digitalWrite(lightSensorInterruptPin, HIGH);
		SGLEDSetFlag(1);
	}
	
	num_runs++;
}


/* Sampling timer and sampling routine */

uint8_t __timer3Control;
uint16_t __timer3CounterValue;

// On 16 MHz Arduino boards, this function has a resolution of 4us
// On 8 MHz Arduino boards, this function has a resolution of 8us
void startTimer3(void) {
	pauseTimer3();
	TCCR3A = 0;
	TCCR3C = 0;
	#if (F_CPU == 16000000L) || (F_CPU == 8000000L)
		// CTC mode, prescaler = 64
		__timer3Control = B00001011;
		__timer3CounterValue = 0;
	#else
		#error("Unsupported CPU frequency")
	#endif
	resetTimer3();
	TIFR3 = 0;
	// Enable compare match 3A interrupt
	TIMSK3 = B00000010;
	// Match at TCNT3 = 4
	OCR3A = 35;
	resumeTimer3();
}
uint16_t readTimer3(void) {
	// 17.3 Accessing 16-bit Registers (page 138)
	uint8_t sreg;
	uint16_t i;
	// Save global interrupt flag
	// 7.4.1 SREG – AVR Status Register (page 14)
	sreg = SREG;
	// Disable interrupts
	cli();
	// Read TCNTn
	i = readTimer3Unsafe();
	// Restore global interrupt flag
	SREG = sreg;
	return i;
}
void resetTimer3(void) {
	// 17.3 Accessing 16-bit Registers (page 138)
	uint8_t sreg;
	// Save global interrupt flag
	// 7.4.1 SREG – AVR Status Register (page 14)
	sreg = SREG;
	// Disable interrupts
	cli();
	// Write TCNTn
	resetTimer3Unsafe();
	// Restore global interrupt flag
	SREG = sreg;
}



void samplingSetup()
{
	startTimer3();
	pauseTimer3();
	sample_flag = 0;
}


ISR(TIMER3_COMPA_vect)
{
	// Handle sampling
	sample_flag = 1;
	//measureLEDRisingEdgeDetection();
}

bool samplingCheckFlag()
{
	if (sample_flag){
		sample_flag = 0;
		return true;
	}
	else{
		return false;
	}
}















































// void measureLEDSetup()
// {
//     // Timer 3 for measuring delay, 64us resolution
//     startUltraSlowCountingTimer3();
//     pauseTimer3();

//     measured_delay_flag = 0;
//     pinMode(lightSensorInterruptPin, OUTPUT);	// Can generate software interrupt
//     digitalWrite(lightSensorInterruptPin, LOW);
//     attachInterrupt(digitalPinToInterrupt(lightSensorInterruptPin), measureLEDDelay, RISING);

//     measureLEDInitializeSamples();
//     num_runs = 0;
// }

// /* Interrupt service routine for external interrupt at pin 3, triggers when light is recieved at sensor */
// void measureLEDDelay()
// {
//     // Stop timer
//     pauseTimer3();

//     // Save delay measurement

//     // readTimer3() returns a maximum value of 65535
//     // That means the maximum possible delta one can measure with this
//     // function (when in ultra slow counting mode) is 4194ms on 16 MHz boards
//     delta = readTimer3();

//     // If you estimate deltaMicros could be > 65 ms, or 65535 us,
//     // delta should be cast to unsigned long, and deltaMicros should be
//     // created as an unsigned long variable
//     deltaMicros = microsFromUltraSlowCounting((unsigned long)delta);

//     measured_delay_flag = 1;
// }




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