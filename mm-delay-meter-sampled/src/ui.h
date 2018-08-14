/* Controls the user interface of the device: LEDs and push buttons */
#ifndef UI_H
#define UI_H

#include <inttypes.h>

void uiSetup();
void setAllLEDs(volatile uint8_t &mode);
void setModeLEDs(volatile uint8_t &mode);

#endif /*UI_H*/