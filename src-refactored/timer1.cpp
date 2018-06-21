/* Timer1 controls the LED on/off and the stopwatch that measures delay.
 * Has a period of 4194 ms before timer overflow, and a resolution of 64us. */

#include "timer1.h"
#include "config.h"

uint8_t __timer1Control;
uint16_t __timer1CounterValue;
volatile bool timer1_ovf_flag;
volatile bool timer1_comp_match_flag;

// On 16 MHz Arduino boards, this function has a resolution of 64us
void startTimer1(void) {
	pauseTimer1();
	TCCR1A = 0;
	TCCR1C = 0;
	#if (F_CPU == 16000000L) || (F_CPU == 8000000L)
		__timer1Control = B00000101;	// prescaler 1024
		__timer1CounterValue = 0;
	#else
		#error("Unsupported CPU frequency")
	#endif
	resetTimer1();
	TIFR1 = 0;
	// Enable timer overflow interrupt and compare match A interrupt:
	TIMSK1 = B00000011;
	// Set compare match register A at 16MHz/1024/0.5Hz
	OCR1A = 31250;	
	resumeTimer1();
}


uint16_t readTimer1(void) {
	// 17.3 Accessing 16-bit Registers (page 138)
	uint8_t sreg;
	uint16_t i;
	// Save global interrupt flag
	// 7.4.1 SREG – AVR Status Register (page 14)
	sreg = SREG;
	// Disable interrupts
	cli();
	// Read ICRn
	i = readTimer1Unsafe();
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

ISR(TIMER1_OVF_vect){
	timer1_ovf_flag = 1;
}

ISR(TIMER1_COMPA_vect){
	timer1_comp_match_flag = 1;
}

bool timer1CheckOvfFlag()
{
	if (timer1_ovf_flag){
		timer1_ovf_flag = 0;
		return true;
	}
	else{
		return false;
	}
}

bool timer1CheckCompFlag()
{
	if (timer1_comp_match_flag){
		timer1_comp_match_flag = 0;
		return true;
	}
	else{
		return false;
	}
}
bool timer1CheckFlag(uint8_t flag_type)
{
	switch (flag_type){
		case OVF:
		if (timer1_ovf_flag){
			timer1_ovf_flag = 0;
			return true;
		}
		else{
			return false;
		}
		break;

		case COMP:
		if (timer1_comp_match_flag){
			timer1_comp_match_flag = 0;
			return true;
		}
		else{
			return false;
		}
		break;
	}
}

void timer1ClearFlags()
{
	timer1_ovf_flag = 0;
	timer1_comp_match_flag = 0;
}