#pragma once

#include <inttypes.h>

#define BUF_SIZE		5

void measurementSamplesSetup(int mode);
bool measurementSamplesCheckMeasuredFlag();
double measurementSamplesGetDelayMs();

void measurementSamplesInitialize();
int16_t measurementSamplesMaxSmoothingFilter();
bool measurementSamplesRisingEdgeDetection(int mode);
bool measurementSamplesRisingEdgeDetectionVideo();
bool measurementSamplesRisingEdgeDetectionSound();

void measurementSamplesClearLightRecievedFlag();
void measurementSamplesClearSoundRecievedFlag();

void setIdleMicVal(int val);
unsigned long measurementSamplesGetSavedSample(uint8_t index);