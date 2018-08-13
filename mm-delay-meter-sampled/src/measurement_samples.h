#pragma once

#include <inttypes.h>
#include "Arduino.h"
#include "datalogger.h"

#define BUF_SIZE		6
#define BUF_PART_1		2
#define BUF_PART_2		4


void measurementSamplesSetup(uint8_t mode);
void measurementSamplesSetMode(uint8_t mode);
bool measurementSamplesCheckMeasuredFlag();

void measurementSamplesInitialize(uint8_t mode);
int16_t measurementSamplesMaxSmoothingFilter();
bool measurementSamplesRisingEdgeDetection(uint8_t mode, bool start_new_series);

void measurementSamplesClearLightRecievedFlag();
void measurementSamplesClearSoundRecievedFlag();

String measurementSamplesGetSavedSample(uint8_t index);

uint8_t getNumMeasurementsCompleted();
void resetNumMeasurementsCompleted();
void resetSavedMeasurements();
void setMeasuredFlag();
void clearMeasuredFlag();

extern Logger Log;