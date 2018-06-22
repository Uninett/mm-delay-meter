#pragma once

#include <inttypes.h>

extern uint8_t __timer3Control;
extern uint16_t __timer3CounterValue;
#define readTimer3Unsafe() TCNT3
#define resetTimer3Unsafe() TCNT3 = __timer3CounterValue
#define pauseTimer3() TCCR3B = 0
#define resumeTimer3() TCCR3B = __timer3Control
extern void startTimer3(void);
extern uint16_t readTimer3(void);
extern void resetTimer3(void);

bool timer3CheckSamplingFlag();
void timer3ClearSamplingFlag();
