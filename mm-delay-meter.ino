#include "src/measure_led.h"
#include "src/signal_generator_timer.h"
#include "src/signal_generator_led.h"
#include "src/signal_generator_speaker.h"
#include "src/Timer3/Timer3.h"
#include <Process.h>

void setup() {
  
  Serial.begin(9600);
  
  while(!Serial);
  Serial.println("Hello Yún!");

//  // Bridge startup
//  Serial.println("Waiting...");
//  Bridge.begin();  // make contact with the linux processor
//  Serial.println("Bridge set up");

  measureLEDSetup();
  SGTimerSetup();
  SGLEDSetup();
  SGSpeakerSetup();
   
//  // get the time that this sketch started:
//  Process startTime;
//  String startString;
//  startTime.runShellCommand("date");
//  while (startTime.available()) {
//    char c = startTime.read();
//    startString += c;
//  }
//  Serial.print("Start time of sketch: ");
//  SERIAL_PORT_USBVIRTUAL.println(startString);
}

void loop() {
  if (SGTimerCheckFlag()){
    SGLEDToggle();
    SGSpeakerPlaySound();
    printSensorValue();
  }
  
  if (measureLEDCheckFlag()){
    //Serial.println("Measurement interrupt");
    printSensorValue();
    printDelay();
    measureLEDPrintToSerial();
  }

  
}
