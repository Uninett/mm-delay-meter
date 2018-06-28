#pragma once

#include <inttypes.h>

#define BUF_SIZE		10

void measurementSamplesSetup(int mode);
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
unsigned long measurementSamplesGetSavedSample(uint8_t index);