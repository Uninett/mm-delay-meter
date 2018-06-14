#pragma once
#include "Arduino.h"

#define ledPin  13

void SGLEDSetup();
void SGLEDToggle();
bool SGLEDCheckFlag();
void SGLEDSetFlag(bool val);
