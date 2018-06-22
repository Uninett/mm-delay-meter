/* Toggles a LED at pin 13 when triggered */

#include "signal_generator_led.h"
#include "input_capture_aco.h"

bool light_recieved_at_sensor;

void SGLEDSetup()
{
	pinMode(ledPin, OUTPUT);
}

void SGLEDToggle()
{
	int prev_ledPin_status = digitalRead(ledPin);
	
	if (prev_ledPin_status == 0){
		// Rising edge: Reset counter of light delay
		// Assume paused (in ISR of measurement)
		// Initialize our counter to zero
		inputCaptureACOSetFlag(LOW);
		resetTimer1();
		resumeTimer1();
		
	}
	digitalWrite(ledPin, prev_ledPin_status ^1);   // toggle LED pin
}

bool SGLEDCheckFlag()
{
	return light_recieved_at_sensor;
}

void SGLEDSetFlag(bool val)
{
	light_recieved_at_sensor = val;
}