#pragma once

#define NUM_SAMPLES 			20
#define lightSensorPin			A0
#define lightSensorInterruptPin	7		

void measureLEDInitializeSamples();
unsigned short measureLEDMaxSmoothingFilter();
void measureLEDRisingEdgeDetection();

void measureLEDSetup();
void measureLEDDelay();
void printSensorValue();
void printDelay();
void printSamples();
bool measureLEDCheckFlag();
void measureLEDPrintToSerial();
