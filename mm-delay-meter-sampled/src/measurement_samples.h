/* Samples the sensors at the interval defined by Timer3
 * and measures delay since a signal was sent to the speaker or LED.
 * Implements a maximum smoothing filter and rising edge detection for detecting 
 * a recieved pulse. */
#ifndef MEASUREMENT_SAMPLES_H
#define MEASUREMENT_SAMPLES_H

#include <inttypes.h>
#include "Arduino.h"
#include "datalogger.h"

#define BUF_SIZE		15
#define BUF_PART_1		5
#define BUF_PART_2		10


void measurementSamplesSetup(uint8_t mode);
void measurementSamplesSetMode(uint8_t mode);

/* Maximum smoothing filter */
void measurementSamplesInitialize(uint8_t mode);
int16_t measurementSamplesMaxSmoothingFilter();

/* Rising edge detection */
bool measurementSamplesRisingEdgeDetection(uint8_t mode, bool start_new_series);

void resetNumMeasurementsCompleted();
uint8_t getNumMeasurementsCompleted();

void setMeasuredFlag();
void clearMeasuredFlag();
bool measurementSamplesCheckMeasuredFlag();

void measurementSamplesClearLightRecievedFlag();
void measurementSamplesClearSoundRecievedFlag();

void resetSavedMeasurements();
String measurementSamplesGetSavedSample(uint8_t index);

extern Logger Log;

#endif /*MEASUREMENT_SAMPLES_H*/