/* Toggles a LED at pin 13 when triggered */

#include "signal_generator_led.h"
#include "signal_generator_timer.h"

unsigned short lastTickCount;

void SGLEDSetup()
{
	pinMode(ledPin, OUTPUT);
}

void SGLEDToggle()
{
	int prev_ledPin_status = digitalRead(ledPin);
	digitalWrite(ledPin, prev_ledPin_status ^1);   // toggle LED pin
	
	if (prev_ledPin_status == 0){
		// if rising edge: Reset counter of light delay

		//resetTimer3();
		// Initialize our counter, or set to zero??
	 	//lastTickCount = readTimer3();
	}

	
}
