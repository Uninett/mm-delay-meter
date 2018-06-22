#pragma once

#include <inttypes.h>

#define analogCompNonInvertedPin	7		// 7: AIN0(AIN+)
#define	analogCompInvertedPin		A5		//A5: AIN-, ADC0

#ifndef microsFromUltraSlowCounting
	#if (F_CPU == 16000000L)
    	#define microsFromUltraSlowCounting(SLOWCOUNTING) ((SLOWCOUNTING) << 6)
	#elif (F_CPU == 8000000L)
    	#define microsFromUltraSlowCounting(SLOWCOUNTING) ((SLOWCOUNTING) << 7)
	#else
    	#error("Unsupported CPU frequency")
	#endif
#endif

extern uint8_t __timer1Control_on;
extern uint8_t __timer1Control_off;
extern uint16_t __timer1CounterValue;
#define readInputCapture1Unsafe() ICR1
#define resetTimer1Unsafe() TCNT1 = __timer1CounterValue
#define pauseTimer1() TCCR1B = __timer1Control_off
#define resumeTimer1() TCCR1B = __timer1Control_on
extern void startUltraSlowCountingTimer1(void);
extern uint16_t readInputCapture1(void);
extern void resetTimer1(void);

extern void inputCaptureACOSetup(void);
extern double inputCaptureACOGetDelayMs(void);
extern int inputCaptureACOGetDelayUs(void);
extern bool inputCaptureACOCheckFlag(void);
extern void inputCaptureACOSetFlag(bool val);
void inputCaptureACOISR();