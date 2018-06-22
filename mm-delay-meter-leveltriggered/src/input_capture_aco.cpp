#include "Arduino.h"
#include "input_capture_aco.h"

volatile bool led_flag;
volatile unsigned short delta;
volatile unsigned long deltaMicros;
uint8_t __timer1Control_on;
uint8_t __timer1Control_off;
uint16_t __timer1CounterValue;
bool input_captured;

void inputCaptureACOSetup()
{

    // Timer 1 for measuring delay, 64us resolution
    startUltraSlowCountingTimer1();
    pauseTimer1();

    // Choose ADC0 as inverted input
    ADCSRA	&= ~(1 << ADEN);
    ADCSRB	|=  (1 << ACME);
    ADMUX	&= ~(1 << MUX2) & ~(1 << MUX1) & ~(1 << MUX0);
    pinMode(analogCompInvertedPin, INPUT);

    // Choose AIN0 as noninverted input
    DIDR1	|=  (1 << AIN0D);	// Disable digital input buffer
    ACSR 	&= ~(1 << ACBG);
    pinMode(analogCompNonInvertedPin, INPUT);

    // Enable analog comparator as source for input capture unit
    ACSR 	|=  (1 << ACIC);

    led_flag = 0;
    input_captured = 0;

}


/* Interrupt service routine for input capture event at ICP1 */
ISR(TIMER1_CAPT_vect){
	
	// Save delay measurement

    // readInputCapture1() returns a maximum value of 65535
    // That means the maximum possible delta one can measure with this
    // function (when in ultra slow counting mode) is 4194ms on 16 MHz boards
	delta = ICR1;
	// If you estimate deltaMicros could be > 65 ms, or 65535 us,
    // delta should be cast to unsigned long, and deltaMicros should be
    // created as an unsigned long variable
    if (!input_captured){
		deltaMicros = microsFromUltraSlowCounting((unsigned long)delta);

		pauseTimer1();
		led_flag = 1;
		input_captured = 1;
	}
}

// On 16 MHz Arduino boards, this function has a resolution of 64us
void startUltraSlowCountingTimer1(void) {
	pauseTimer1();
	TCCR1A = 0;
	TCCR1C = 0;
	#if (F_CPU == 16000000L) || (F_CPU == 8000000L)
		__timer1Control_on = B11000101;	// input capture noise canceler and rising edge select, prescaler 1024
		__timer1Control_off = B11000000;
		__timer1CounterValue = 0;
	#else
		#error("Unsupported CPU frequency")
	#endif
	resetTimer1();
	TIFR1 = 0;
	TIMSK1 = B00100000;					// input capture enable
	resumeTimer1();
}


uint16_t readInputCapture1(void) {
	// 17.3 Accessing 16-bit Registers (page 138)
	uint8_t sreg;
	uint16_t i;
	// Save global interrupt flag
	// 7.4.1 SREG – AVR Status Register (page 14)
	sreg = SREG;
	// Disable interrupts
	cli();
	// Read ICRn
	i = readInputCapture1Unsafe();
	// Restore global interrupt flag
	SREG = sreg;
	return i;
}

void resetTimer1(void) {
	// 17.3 Accessing 16-bit Registers (page 138)
	uint8_t sreg;
	// Save global interrupt flag
	// 7.4.1 SREG – AVR Status Register (page 14)
	sreg = SREG;
	// Disable interrupts
	cli();
	// Write TCNTn
	resetTimer1Unsafe();
	// Restore global interrupt flag
	SREG = sreg;
}



bool inputCaptureACOCheckFlag()
{
	if (led_flag){ 
    	led_flag = 0;
    	return true;
 	}
 	else{
    	return false;
 	}
}


double inputCaptureACOGetDelayMs()
{
	return deltaMicros/1000.0;
}

int inputCaptureACOGetDelayUs()
{
	return deltaMicros;
}

void inputCaptureACOSetFlag(bool val)
{
	input_captured = val;
}
