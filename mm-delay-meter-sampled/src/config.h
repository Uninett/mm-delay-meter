#pragma once

//#define speakerPin  				11	// OC0A
#define modeSelectPin				2	// INT1
#define startPin					3	// INT0
#define buzzerPin					8	// Try changing to pin 4,5,6
#define statusLedPin1				9
#define soundModeIndicator			10	// statusLedPin2
#define videoModeIndicator			11	// statusLedPin3
#define ledPin  					12
#define startIndicator				13	// Change to 8. Green LED
#define lightSensorPin				A0
#define microphonePin				A1

//#define testFreqPin					6

#define NUM_SAMPLES 				10

#define VIDEO_MODE		            1
#define SOUND_MODE		            2

#include "Arduino.h"
