#pragma once

#include <inttypes.h>

void measurementSamplesSetup();
bool measurementSamplesCheckMeasuredFlag();
double measurementSamplesGetDelayMs();

void measurementSamplesInitialize();
int16_t measurementSamplesMaxSmoothingFilter();
void measurementSamplesRisingEdgeDetection();

void measurementSamplesClearLightRecievedFlag();