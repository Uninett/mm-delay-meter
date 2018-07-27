#pragma once

#include <inttypes.h>
#include "Arduino.h"

#define BUF_SIZE		5

void measurementSamplesSetup(uint8_t mode);
void measurementSamplesSetMode(uint8_t mode);
bool measurementSamplesCheckMeasuredFlag();

void measurementSamplesInitialize(uint8_t mode);
int16_t measurementSamplesMaxSmoothingFilter();
bool measurementSamplesRisingEdgeDetection(int mode);
bool measurementSamplesRisingEdgeDetectionVideo();
bool measurementSamplesRisingEdgeDetectionSound();

void measurementSamplesClearLightRecievedFlag();
void measurementSamplesClearSoundRecievedFlag();

String measurementSamplesGetSavedSample(uint8_t index);