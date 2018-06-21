#pragma once

#include <inttypes.h>

extern volatile unsigned long deltaMicros;
#define measurementSamplesGetDelayMs() deltaMicros/1000.0

void measurementSamplesSetup();
bool measurementSamplesCheckMeasuredFlag();

void measurementSamplesInitialize();
uint16_t measurementSamplesMaxSmoothingFilter();
void measurementSamplesRisingEdgeDetection();

void measurementSamplesClearLightRecievedFlag();