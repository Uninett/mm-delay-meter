#pragma once

#define NUM_SAMPLES 			20
#define NUM_MAX_SAMPLES			10
#define lightSensorPin			A0
#define lightSensorInterruptPin	7		

void measureLEDInitializeSamples();
unsigned short measureLEDMaxSmoothingFilter();
void measureLEDRisingEdgeDetection();

void measureLEDSetup();
void measureLEDDelay();
//void printSensorValue();
//void printDelay();
bool measureLEDCheckFlag();
void measureLEDPrintToSerial();
double measureLEDGetDelayMs();
