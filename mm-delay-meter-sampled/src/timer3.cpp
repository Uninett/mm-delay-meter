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

/************************************************/
#include "timer3.h"
#include "config.h"

uint8_t __timer3Control;
uint16_t __timer3CounterValue;
volatile bool timer3_sample_flag;

// On 16 MHz Arduino boards, this function has a resolution of 4us
void startTimer3(int mode)
/* Set timer registers and start counter at 0 */
{
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
	// Sound mode needs higher sampling frequency because 
	// the voltage from the microphone oscillates with high frequency
	if (mode == SOUND_MODE)	OCR3A = 7;  // f = 1/28us = 35 714 Hz
	if (mode == VIDEO_MODE) OCR3A = 10; // f = 1/40us = 25 kHz
	resumeTimer3();
}

void resetTimer3(void)
/* Reset TCNT3 to 0 while making sure nothing can interrupt the resetting */
{
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

uint16_t readTimer3(void)
/* Read the value in TCNT3 while making sure nothing can interrupt the read */
{
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
/***************************************************/


bool timer3CheckSamplingFlag()
/* Check for a timer 3 interrupt that signals a sample should be taken */
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
/* Signal that the timer 3 interrupt has been handled */
{
	timer3_sample_flag = 0;
}

ISR(TIMER3_COMPA_vect)
{
	timer3_sample_flag = 1;
}