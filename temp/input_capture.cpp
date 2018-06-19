

#include "Arduino.h"
#include "input_capture.h"

volatile bool led_flag;
volatile unsigned short delta;
volatile unsigned long deltaMicros;
uint8_t __timer3Control_on;
uint8_t __timer3Control_off;
uint16_t __timer3CounterValue;
bool input_captured;

void inputCaptureSetup()
{

    // Timer 3 for measuring delay, 64us resolution
    startUltraSlowCountingTimer3();
    pauseTimer3();

    led_flag = 0;
    input_captured = 0;
    pinMode(lightInputCapturePin, INPUT);

}


/* Interrupt service routine for input capture event at ICP3 */
ISR(TIMER3_CAPT_vect){
	
	// Save delay measurement

    // readInputCapture3() returns a maximum value of 65535
    // That means the maximum possible delta one can measure with this
    // function (when in ultra slow counting mode) is 4194ms on 16 MHz boards
	delta = ICR3;
		// If you estimate deltaMicros could be > 65 ms, or 65535 us,
	    // delta should be cast to unsigned long, and deltaMicros should be
	    // created as an unsigned long variable
    if (!input_captured){
		deltaMicros = microsFromUltraSlowCounting((unsigned long)delta);

		pauseTimer3();
		led_flag = 1;
		input_captured = 1;
	}
}

// On 16 MHz Arduino boards, this function has a resolution of 64us
// On 8 MHz Arduino boards, this function has a resolution of 128us
void startUltraSlowCountingTimer3(void) {
	pauseTimer3();
	TCCR3A = 0;
	TCCR3C = 0;
	#if (F_CPU == 16000000L) || (F_CPU == 8000000L)
		__timer3Control_on = B11000101;	// input capture noise canceler and rising edge select
		__timer3Control_off = B11000000;
		__timer3CounterValue = 0;
	#else
		#error("Unsupported CPU frequency")
	#endif
	resetTimer3();
	TIFR3 = 0;
	TIMSK3 = B00100000;					// input capture enable
	resumeTimer3();
}


uint16_t readInputCapture3(void) {
	// 17.3 Accessing 16-bit Registers (page 138)
	uint8_t sreg;
	uint16_t i;
	// Save global interrupt flag
	// 7.4.1 SREG – AVR Status Register (page 14)
	sreg = SREG;
	// Disable interrupts
	cli();
	// Read ICRn
	i = readInputCapture3Unsafe();
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



bool inputCaptureCheckFlag()
{
	if (led_flag){ 
    	led_flag = 0;
    	return true;
 	}
 	else{
    	return false;
 	}
}


double inputCaptureGetDelayMs()
{
	return deltaMicros/1000.0;
}

int inputCaptureGetDelayUs()
{
	return deltaMicros;
}

void inputCaptureSetFlag(bool val)
{
	input_captured = val;
}
