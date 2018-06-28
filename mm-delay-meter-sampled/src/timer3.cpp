/* Timer3 controls the measurement sample interval */

#include "timer3.h"
#include "config.h"

uint8_t __timer3Control;
uint16_t __timer3CounterValue;
volatile bool timer3_sample_flag;

// On 16 MHz Arduino boards, this function has a resolution of 4us
void startTimer3(void) {
	/*TEST*/
	pinMode(testFreqPin, OUTPUT);

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
	// Match at TCNT3 = 8: 32us
	// Sampling frequency: 1/0.000032s = 31 250 Hz
	OCR3A = 10; 
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

ISR(TIMER3_COMPA_vect)
{
	timer3_sample_flag = 1;
}

bool timer3CheckSamplingFlag()
{
	if (timer3_sample_flag){
		timer3_sample_flag = 0;
		return true;
	}
	else{
		return false;
	}
}

void timer3ClearSamplingFlag()
{
	timer3_sample_flag = 0;
}