#include "signal_generator.h"
#include "timer1.h"
#include "measurement_samples.h"
#include "config.h"

void signalGeneratorSetup()
/* Get signal generator ready: set pin directions, set up timer and clear flags */
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
/* Reset delay timer to zero and turn on LED */
{	
	measurementSamplesClearLightRecievedFlag();
	resetTimer1();	
	digitalWrite(ledPin, HIGH);
}

void signalGeneratorLEDOff()
/* Turn off LED */
{
	digitalWrite(ledPin, LOW);
}

void signalGeneratorStatusLEDControl()
/* Control the three blinking LEDs that indicate how many measurements have been made */
{
	uint8_t num_measurements = getNumMeasurementsCompleted();
	if (num_measurements > 0 && num_measurements < BUF_PART_1){
		// Blink statusLed1
		digitalWrite(statusLedPin1, LOW);
	}
	else if (num_measurements > BUF_PART_1 && num_measurements < BUF_PART_2){
		// Blink statusLed2
		digitalWrite(soundModeIndicator, LOW);
	}
	else if (num_measurements > BUF_PART_2 && num_measurements < BUF_SIZE){
		// Blink statusLed3
		digitalWrite(videoModeIndicator, LOW);
	}
}

void signalGeneratorSpeakerOn()
/* Reset delay timer to zero and turn on buzzer */
{
	measurementSamplesClearSoundRecievedFlag();
	resetTimer1();
	digitalWrite(buzzerPin, HIGH);
}

void signalGeneratorSpeakerOff()
/* Turn off buzzer */
{
	digitalWrite(buzzerPin, LOW);
}