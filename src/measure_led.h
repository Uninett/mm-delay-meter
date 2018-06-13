#pragma once

#define NUM_SAMPLES 			10
#define lightSensorPin			A0
#define lightSensorInterruptPin	7		

// void lightSetup();
// void initializeSamples();
// void readLightInput();
// int findMax();
// int maxSmoothingFilter();
// bool risingEdgeDetection();

void measureLEDSetup();
void measureLEDDelay();
void printSensorValue();
void printDelay();
bool measureLEDCheckFlag();
