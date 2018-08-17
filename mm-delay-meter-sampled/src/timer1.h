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

/***********************************************************************************/
/* Timer1 controls the LED and speaker on/off and the stopwatch that measures delay.
 * Has a period of 4194 ms before timer overflow, and a resolution of 64us. */
#ifndef TIMER1_H
#define TIMER1_H

#include <inttypes.h>

#define OVF		1
#define COMP	2

#ifndef microsFromCounting
	#if (F_CPU == 16000000L)
    	#define microsFromCounting(SLOWCOUNTING) ((SLOWCOUNTING) << 6)
	#elif (F_CPU == 8000000L)
    	#define microsFromCounting(SLOWCOUNTING) ((SLOWCOUNTING) << 7)
	#else
    	#error("Unsupported CPU frequency")
	#endif
#endif

extern uint8_t __timer1Control;
extern uint16_t __timer1CounterValue;
#define readTimer1Unsafe() TCNT1
#define resetTimer1Unsafe() TCNT1 = __timer1CounterValue
#define pauseTimer1() TCCR1B = 0
#define resumeTimer1() TCCR1B = __timer1Control
extern void startTimer1(void);
extern void resetTimer1(void);
extern uint16_t readTimer1(void);

bool timer1CheckFlag(uint8_t flag_type);
void timer1ClearFlags();

#endif /*TIMER1_H*/