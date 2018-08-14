#include "ui.h"
#include "config.h"

void uiSetup()
{
	/* LEDs */
	pinMode(statusLedPin1, OUTPUT);
	pinMode(soundModeIndicator, OUTPUT);
	pinMode(videoModeIndicator, OUTPUT);
	pinMode(startIndicator, OUTPUT);
	digitalWrite(statusLedPin1, LOW);
	digitalWrite(soundModeIndicator, LOW);
	digitalWrite(videoModeIndicator, HIGH);
	digitalWrite(startIndicator, LOW);

	/* Buttons */
	pinMode(modeSelectPin, INPUT);
	pinMode(startPin, INPUT);
	// Configure external interrupt INT0, rising edge, D3
	// Start/stop button
	EICRA |= (1 << ISC01) | (1 << ISC00);
	EIMSK |= (1 << INT0); 
	// Configure external interrupt INT1, edge, D2
	// Mode button
	EICRA &= ~(1 << ISC11);
	EICRA |= (1 << ISC10);
	EIMSK |= (1 << INT1); 
}

void setModeLEDs(volatile uint8_t &mode)
{
	if (mode == VIDEO_MODE){
		digitalWrite(videoModeIndicator, HIGH);
		digitalWrite(soundModeIndicator, LOW); 
	} 
	else{
		digitalWrite(videoModeIndicator, LOW);
		digitalWrite(soundModeIndicator, HIGH); 
	}
}

void setAllLEDs(volatile uint8_t &mode)
{
	digitalWrite(startIndicator, LOW);
	setModeLEDs(mode);
	digitalWrite(statusLedPin1, LOW);
}
