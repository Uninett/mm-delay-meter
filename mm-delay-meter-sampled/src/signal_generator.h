#pragma once

#define pauseTimer0() TCCR0B = 0
#define resumeTimer0() TCCR0B = B00000011
#define resetTimer0Unsafe() TCNT0 = 0

void signalGeneratorSetup();
void signalGeneratorLEDOn();
void signalGeneratorLEDOff();
void signalGeneratorSpeakerOn();
void signalGeneratorSpeakerOff();