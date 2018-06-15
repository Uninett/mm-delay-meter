#include "src/measure_led.h"
#include "src/signal_generator_timer.h"
#include "src/signal_generator_led.h"
#include "src/signal_generator_speaker.h"
#include "src/Timer3/Timer3.h"
#include "src/sd_card_datalogger.h"
#include <Process.h>

#define NUM_MEASUREMENTS  10
int num_measurements;

void setup() {
  
  Serial.begin(9600);
  
  while(!Serial);
  Serial.println("Hello Yún!");

//  // Bridge startup
  Bridge.begin();
  Serial.println("Bridge setup complete");
  measureLEDSetup();
  SGTimerSetup();
  SGLEDSetup();
  SGSpeakerSetup();
  
  SDCardSetup();

  num_measurements = 0;

//  String result;
//  Process time;
//  // date is a command line utility to get the date and the time
//  // in different formats depending on the additional parameter
//  time.runShellCommand("date");
//
//  // read the output of the command
//  while (time.available() > 0) {
//    char c = time.read();
//    result += c;
//  }
//  Serial.println(result);
}

void loop() {
  if (SGTimerCheckFlag()){
    SGLEDToggle();
    SGSpeakerPlaySound();
  }
  measureLEDRisingEdgeDetection();

  if (measureLEDCheckFlag()){
    digitalWrite(lightSensorInterruptPin, LOW);
    //measureLEDPrintToSerial();
    /* Save values in SD card */
    SDCardLogger("measurements.txt");
    num_measurements++;
  }
  if (num_measurements >= NUM_MEASUREMENTS){
    /* Save measurements somewhere, try putty */
    digitalWrite(ledPin, LOW);
    while(1){};
  
  }
 
}
