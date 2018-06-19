#pragma once

#include <inttypes.h>

#define lightInputCapturePin	13		//13: ICR, 7: interrupt

#ifndef microsFromUltraSlowCounting
	#if (F_CPU == 16000000L)
    	#define microsFromUltraSlowCounting(SLOWCOUNTING) ((SLOWCOUNTING) << 6)
	#elif (F_CPU == 8000000L)
    	#define microsFromUltraSlowCounting(SLOWCOUNTING) ((SLOWCOUNTING) << 7)
	#else
    	#error("Unsupported CPU frequency")
	#endif
#endif

extern uint8_t __timer3Control_on;
extern uint8_t __timer3Control_off;
extern uint16_t __timer3CounterValue;
#define readInputCapture3Unsafe() ICR3
#define resetTimer3Unsafe() TCNT3 = __timer3CounterValue
#define pauseTimer3() TCCR3B = __timer3Control_off
#define resumeTimer3() TCCR3B = __timer3Control_on
extern void startUltraSlowCountingTimer3(void);
extern uint16_t readInputCapture3(void);
extern void resetTimer3(void);

extern void inputCaptureSetup(void);
extern double inputCaptureGetDelayMs(void);
extern int inputCaptureGetDelayUs(void);
extern bool inputCaptureCheckFlag(void);
extern void inputCaptureSetFlag(bool val);
void inputCaptureISR();