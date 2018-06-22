/* Simple timer that sets a flag every other second, using Timer1 on ATmega32U4 (16bit) */
//TODO:: Implement "random" intervals/interrupts somehow (?)

#include "signal_generator_timer3.h"
#include "Arduino.h"
volatile int out_flag;


void SGTimer3Setup()
{
	out_flag = 0;

	// initialize timer3
	noInterrupts();                         // disable all interrupts
	TCCR3A = 0;                             // clear registers
	TCCR3B = 0;
	TCNT3  = 0;

	OCR3A = 31250;                          // compare match register 16MHz/1024/0.5Hz
	TCCR3B |= (1 << WGM32);                 // CTC mode
	TCCR3B |= (1 << CS32) | (1 << CS30);    // 1024 prescaler 
	TIMSK3 |= (1 << OCIE3A);                // enable timer compare interrupt
	interrupts();                           // enable all interrupts
}

/* timer1 compare interrupt service routine */
ISR(TIMER3_COMPA_vect)
{
	out_flag = 1;
}

bool SGTimer3CheckFlag(){
 	if (out_flag){ 
    	out_flag = 0;
    	return true;
 	}
 	else{
    	return false;
 	}
}

