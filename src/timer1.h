#pragma once

#include <inttypes.h>


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
extern uint16_t readTimer1(void);
extern void resetTimer1(void);

void timer1ControlSetup();
bool timer1CheckResetFlag();
void timer1ClearResetFlag();