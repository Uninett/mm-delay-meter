#pragma once


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
