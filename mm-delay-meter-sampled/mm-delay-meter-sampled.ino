#include "src/config.h"
#include "src/timer1.h"
#include "src/timer3.h"
#include "src/signal_generator.h"
#include "src/measurement_samples.h"
#include "src/sd_card_datalogger.h"
#include <Process.h>

#define MAX_NUM_MEASUREMENTS  20
int num_measurements;
const int mode = VIDEO_MODE;
bool started_measurements = false;
String date;
String start_time;
Process p;

void setup() {
  
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Hello Yún!");

  // Bridge startup
  Serial.println("Bridge...");
  Bridge.begin();
  Serial.println("Bridge setup complete");
  delay(3000);
  digitalWrite(LED_BUILTIN, HIGH);

  SDCardSetup();
  signalGeneratorSetup(mode);
  measurementSamplesSetup(mode);

  resumeTimer1();
  resumeTimer3();
  //ADD: blink when ready (after 20-30 sec?)
  
}

void loop() {
  if (timer1CheckFlag(OVF)){
    signalGeneratorLEDOn();
    if (mode == SOUND_MODE){
      signalGeneratorSpeakerOn();
    }
  }
  if (timer1CheckFlag(COMP)){
    signalGeneratorLEDOff();
    if (mode == SOUND_MODE){
      signalGeneratorSpeakerOff();
    }
  }
  if (timer3CheckSamplingFlag()){
    bool first_edge = measurementSamplesRisingEdgeDetection(mode);
    if (!started_measurements && first_edge){
      // Get start time
      p.begin("date");
      p.addParameter("+%F");
      p.run();
      date = "";
      while(p.available() > 0) {
        char c = p.read();
        date += c;
      }
      p.begin("date");
      p.addParameter("+%H:%M");
      p.run();
      start_time = "";
      while(p.available() > 0) {
        char c = p.read();
        start_time += c;
      }
      started_measurements = true;
    }
  }
  if (measurementSamplesCheckMeasuredFlag()){
    pauseTimer1();
    pauseTimer3();
    signalGeneratorLEDOff();
    if (mode == SOUND_MODE){
      signalGeneratorSpeakerOff();
    }
    String file = SDCardLogger(start_time, date);
    // Upload to database
    p.runShellCommand("curl --data \"nokkel=A8:40:41:19:74:90\" --data-urlencode seriedata@" + file + " http://delay.uninett.no/fmaling/dbm.php");
    while(p.running());
    String data = "";
    int i = 0;
    while(p.available() > 0){
      char c = p.read();
      data += c;
      if (i > 0 && c != "[" && c != "]"){
        // Successful upload
        p.runShellCommand("rm " + file);
        break;
      }
      i++;
    }
    Serial.println(data);
    while(true);    // TEMPORARY: Stop when a measurement series is complete
    //SDCardPrintContent();
    resumeTimer1();
    resumeTimer3();
  }
}
