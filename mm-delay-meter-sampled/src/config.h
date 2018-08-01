#pragma once

/* Buttons */
#define modeSelectPin				2	// INT1
#define startPin					3	// INT0

/* Status LEDs */
#define startIndicator				8	// Change from 13 to 8. Green LED
#define statusLedPin1				9
#define soundModeIndicator			10	// statusLedPin2
#define videoModeIndicator			11	// statusLedPin3

/* Sensors */
#define lightSensorPin				A0
#define microphonePin				A1

/* Signals */
#define buzzerPin					6	// Try changing from pin 8 to 4,5,6
#define ledPin  					12

//#define testFreqPin					6

#define NUM_SAMPLES 				10
#define VIDEO_MODE		            1
#define SOUND_MODE		            2

#include "Arduino.h"
