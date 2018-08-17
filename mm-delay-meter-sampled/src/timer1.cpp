//
// ArduinoTimer is distributed under the FreeBSD License
//
// Copyright (c) 2013, Carlos Rafael Gimenes das Neves
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.
//
// https://github.com/carlosrafaelgn/ArduinoTimer
//

/***************************************************/
#include "timer1.h"
#include "config.h"

uint8_t __timer1Control;
uint16_t __timer1CounterValue;
volatile bool timer1_ovf_flag;
volatile bool timer1_comp_match_flag;

// On 16 MHz Arduino boards, this function has a resolution of 64us
void startTimer1(void)
/* Set timer registers and start counter at 0 */
{
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

void resetTimer1(void)
/* Reset TCNT1 to 0 while making sure nothing can interrupt the resetting */
{
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

uint16_t readTimer1(void)
/* Read the value in TCNT1 while making sure nothing can interrupt the read */
{
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
/***************************************************/


bool timer1CheckFlag(uint8_t flag_type)
/* Check for a timer 1 interrupt of the specified type */
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
/* Signal that the timer 1 interrupt has been handled */
{
	timer1_ovf_flag = 0;
	timer1_comp_match_flag = 0;
}

ISR(TIMER1_OVF_vect){
	timer1_ovf_flag = 1;
}

ISR(TIMER1_COMPA_vect){
	timer1_comp_match_flag = 1;
}