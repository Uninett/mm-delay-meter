#include "src/config.h"
#include "src/timer1.h"
#include "src/timer3.h"
#include "src/signal_generator.h"
#include "src/measurement_samples.h"
#include "src/sd_card_datalogger.h"
#include <Process.h>

#define MAX_NUM_MEASUREMENTS  20
int num_measurements;
const int mode = SOUND_MODE;

void setup() {
  
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Hello Yún!");

  // Bridge startup
  Bridge.begin();
  Serial.println("Bridge setup complete");

  SDCardSetup();
  signalGeneratorSetup(mode);
  measurementSamplesSetup();
  
  

  // Calibrate microphone measurements
  if(mode == SOUND_MODE){
    Serial.println("Calibrating microphone...");
    int prev_mic;
    int current_mic = analogRead(microphonePin);
    prev_mic = current_mic;
    int mean = 0;
    int mic[10];
    int idle_mic;
    int idle_iterations = 0;
    while (idle_iterations < 10){
      current_mic = analogRead(microphonePin);
      if (abs(current_mic - prev_mic) < 5){
        Serial.print(idle_iterations);
        Serial.print("\t");
        Serial.print(prev_mic);
        Serial.print("\t");
        Serial.println(current_mic);
        mic[idle_iterations] = current_mic;
        idle_iterations++;
      }
      else{
        Serial.println("Reset");
        for (auto &m: mic){
          m = 0;
        }
        idle_iterations = 0;
      }
      prev_mic = current_mic;
    }

    idle_mic = current_mic;
    for (auto &m: mic){
      mean += m;
    }
    mean = mean/10;    
    setIdleMicVal(mean);
    Serial.print("Mean: ");
    Serial.println(mean);
  }

  resumeTimer1();
  resumeTimer3();
  num_measurements = 0;
  
}

void loop() {
  if (timer1CheckFlag(OVF)){
    // Turn on led and start stopwatch
    signalGeneratorLEDOn();
    // Turn on speaker
    if (mode == SOUND_MODE){
      signalGeneratorSpeakerOn();
    }
  }
  if (timer1CheckFlag(COMP)){
    // Turn off LED
    signalGeneratorLEDOff();
    // Turn off speaker
    if (mode == SOUND_MODE){
      signalGeneratorSpeakerOff();
    }
  }
  if (timer3CheckSamplingFlag()){
    // Measure light
    measurementSamplesRisingEdgeDetection(mode);
    // Measure sound
    //measurementSamplesRisingEdgeDetectionSound();
  }
  if (measurementSamplesCheckMeasuredFlag()){
    SDCardLogger("measurements.txt", num_measurements);
    num_measurements++;
  }
}
