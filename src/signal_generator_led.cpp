/* Toggles a LED at pin 13 when triggered */

#include "signal_generator_led.h"
#include "signal_generator_timer.h"
#include "Timer3/Timer3.h"
//#include "input_capture.h"

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
		light_recieved_at_sensor = 0;
		//inputCaptureSetFlag(LOW);
		resetTimer3();
		resumeTimer3();
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