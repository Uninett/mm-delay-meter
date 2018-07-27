#pragma once

#include <inttypes.h>
#include "Arduino.h"

#define BUF_SIZE		5

void measurementSamplesSetup(uint8_t mode);
void measurementSamplesSetMode(uint8_t mode);
bool measurementSamplesCheckMeasuredFlag();

void measurementSamplesInitialize(uint8_t mode);
int16_t measurementSamplesMaxSmoothingFilter();
bool measurementSamplesRisingEdgeDetection(uint8_t mode, bool start_new_series);
bool measurementSamplesRisingEdgeDetectionVideo(bool start_new_series);
bool measurementSamplesRisingEdgeDetectionSound(bool start_new_series);

void measurementSamplesClearLightRecievedFlag();
void measurementSamplesClearSoundRecievedFlag();

String measurementSamplesGetSavedSample(uint8_t index);

uint8_t getNumMeasurementsCompleted();
void resetNumMeasurementsCompleted();
void resetSavedMeasurements();
void setMeasuredFlag();
void clearMeasuredFlag();

