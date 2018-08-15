/* Samples the sensors at the interval defined by Timer3
 * and measures delay since a signal was sent to the speaker or LED.
 * Implements a maximum smoothing filter and rising edge detection for detecting 
 * a recieved pulse. */
#ifndef SENSOR_SAMPLES_H
#define SENSOR_SAMPLES_H

#include <inttypes.h>
#include "Arduino.h"
#include "datalogger.h"

#define BUF_SIZE		15
#define BUF_PART_1		5
#define BUF_PART_2		10


void sensorSamplesSetup(uint8_t mode);
void sensorSamplesSetMode(uint8_t mode);

/* Maximum smoothing filter */
void sensorSamplesInitialize(uint8_t mode);

/* Rising edge detection */
bool sensorSamplesRisingEdgeDetection(uint8_t mode, bool start_new_series);

void resetNumMeasurementsCompleted();
uint8_t getNumMeasurementsCompleted();

void setMeasuredCompleteFlag();
void clearMeasuredCompleteFlag();
bool checkMeasuredCompleteFlag();

void clearLightRecievedFlag();
void clearSoundRecievedFlag();

void resetSavedMeasurements();
String getSavedMeasurement(uint8_t index);

extern Logger Log;

#endif /*SENSOR_SAMPLES_H*/