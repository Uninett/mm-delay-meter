#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

/* Buttons */
#define modeSelectPin				2	// INT1
#define startPin					3	// INT0

/* Status LEDs */
#define startIndicator				8
#define statusLedPin1				9
#define soundModeIndicator			10	// statusLedPin2
#define videoModeIndicator			11	// statusLedPin3

/* Sensors */
#define lightSensorPin				A0
#define microphonePin				A1

/* Signals */
#define buzzerPin					6
#define ledPin  					12

#define NUM_SAMPLES 				10
#define VIDEO_MODE		            1
#define SOUND_MODE		            2

#endif /*CONFIG_H*/