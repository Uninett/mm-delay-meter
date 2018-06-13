#include "src/measure_led.h"
#include "src/signal_generator_timer.h"
#include "src/signal_generator_led.h"
#include "src/signal_generator_speaker.h"
#include "src/Timer3/Timer3.h"

void setup() {
  measureLEDSetup();
  SGTimerSetup();
  SGLEDSetup();
  SGSpeakerSetup();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Hello Yún!");
}

void loop() {
  if (SGTimerCheckFlag()){
    SGLEDToggle();
    SGSpeakerPlaySound();
  }
  
  if (measureLEDCheckFlag()){
    Serial.println("Measurement interrupt");
    printSensorValue();
    printDelay();
  }

  
}
