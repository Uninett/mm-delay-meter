/* Generates the speaker and LED signals.
 * Timer 1 controls the on/off interval. */

#include "signal_generator.h"
#include "timer1.h"
#include "measurement_samples.h"
#include "config.h"

void signalGeneratorSetup(int mode)
{
	timer1ClearFlags();
	startTimer1();
	pauseTimer1();

	pinMode(buzzerPin, OUTPUT);
	signalGeneratorSpeakerOff();
	pinMode(ledPin, OUTPUT);
	signalGeneratorLEDOff();
}

void signalGeneratorLEDOn()
{	
	measurementSamplesClearLightRecievedFlag();
	resetTimer1();		// Redundant? It has already overflowed		
	digitalWrite(ledPin, HIGH);
}

void signalGeneratorLEDOff()
{
	digitalWrite(ledPin, LOW);
}

void signalGeneratorSpeakerOn()
{
	measurementSamplesClearSoundRecievedFlag();
	resetTimer1();
	digitalWrite(buzzerPin, HIGH);
}

void signalGeneratorSpeakerOff()
{
	digitalWrite(buzzerPin, LOW);
}