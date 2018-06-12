#include "measure_led.h"
#include "signal_generator_timer.h"
#include "signal_generator_led.h"
#include "signal_generator_speaker.h"
#include "Timer3/Timer3.h"

void setup() {
  // put your setup code here, to run once:
  //measureLEDSetup();
  SGTimerSetup();
  SGLEDSetup();
  SGSpeakerSetup();
  //startSlowCountingTimer3();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Hello Yún!");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (SGTimerCheckFlag()){
    SGLEDToggle();
    SGSpeakerPlaySound();
  }
  //printSensorValue();
  
}
