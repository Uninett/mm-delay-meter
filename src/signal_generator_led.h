#pragma once
#include "Arduino.h"

#define ledPin  12

void SGLEDSetup();
void SGLEDToggle();
bool SGLEDCheckFlag();
void SGLEDSetFlag(bool val);
