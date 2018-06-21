/* Simple timer that sets a flag every other second, using Timer1 on ATmega32U4 (16bit) */
//TODO:: Implement "random" intervals/interrupts somehow (?)

#include "signal_generator_timer.h"
#include "Arduino.h"
#include "config.h"
volatile int out_flag;


void SGTimerSetup()
{
	out_flag = 0;

	// initialize timer1 
	noInterrupts();                         // disable all interrupts
	TCCR1A = 0;                             // clear registers
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 31250;                          // compare match register 16MHz/1024/0.5Hz
	TCCR1B |= (1 << WGM12);                 // CTC mode
	TCCR1B |= (1 << CS12) | (1 << CS10);    // 1024 prescaler 
	TIMSK1 |= (1 << OCIE1A);                // enable timer compare interrupt
	interrupts();                           // enable all interrupts
}

/* timer1 compare interrupt service routine */
ISR(TIMER1_COMPA_vect)
{
	out_flag = 1;
}

bool SGTimerCheckFlag(){
 	if (out_flag){ 
    	out_flag = 0;
    	return true;
 	}
 	else{
    	return false;
 	}
}

