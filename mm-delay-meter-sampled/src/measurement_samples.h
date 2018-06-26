#pragma once

#include <inttypes.h>

void measurementSamplesSetup();
bool measurementSamplesCheckMeasuredFlag();
double measurementSamplesGetDelayMs();

void measurementSamplesInitialize();
int16_t measurementSamplesMaxSmoothingFilter();
void measurementSamplesRisingEdgeDetection(int mode);
void measurementSamplesRisingEdgeDetectionVideo();
void measurementSamplesRisingEdgeDetectionSound();

void measurementSamplesClearLightRecievedFlag();
void measurementSamplesClearSoundRecievedFlag();

void setIdleMicVal(int val);