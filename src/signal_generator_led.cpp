/* Toggles a LED at pin 13 when triggered */

#include "signal_generator_led.h"
//#include "Timer3/Timer3.h"
#include "timer1.h"
#include "config.h"
#include "Arduino.h"

bool light_recieved_at_sensor;

void SGLEDSetup()
{
	pinMode(ledPin, OUTPUT);
}

void SGLEDOn()
{
	light_recieved_at_sensor = 0;
	resetTimer1();		// Redundant? It has already overflowed
	//resumeTimer1();
		
	digitalWrite(ledPin, HIGH);
}

bool SGLEDCheckFlag()
{
	return light_recieved_at_sensor;
}

void SGLEDSetFlag(bool val)
{
	light_recieved_at_sensor = val;
}





















// void SGLEDToggle()
// {
// 	int prev_ledPin_status = digitalRead(ledPin);
	
// 	if (prev_ledPin_status == 0){
// 		// Rising edge: Reset counter of light delay
// 		// Assume paused (in ISR of measurement)
// 		// Initialize our counter to zero
// 		light_recieved_at_sensor = 0;
// 		resetTimer3();
// 		resumeTimer3();
		
// 	}
// 	digitalWrite(ledPin, prev_ledPin_status ^1);   // toggle LED pin
// }