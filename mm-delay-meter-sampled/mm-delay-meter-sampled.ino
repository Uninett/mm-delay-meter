#include "src/config.h"
#include "src/timer1.h"
#include "src/timer3.h"
#include "src/signal_generator.h"
#include "src/measurement_samples.h"
#include "src/sd_card_datalogger.h"
#include <Process.h>

#define MAX_NUM_MEASUREMENTS  20
int num_measurements;
int mode = VIDEO_MODE;
bool mode_change_flag = false;
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

  /* SETUP */
  pinMode(videoModeIndicator, OUTPUT);
  pinMode(soundModeIndicator, OUTPUT);
  pinMode(modeSelectPin, INPUT);
  digitalWrite(videoModeIndicator, HIGH);
  digitalWrite(soundModeIndicator, LOW);
  // Configure external interrupt INT6, rising edge
  EICRB |= (1 << ISC61) | (1 << ISC60);
  EIMSK |= (1 << INT6);
  interrupts();

  SDCardSetup();
  signalGeneratorSetup(mode);
  measurementSamplesSetup(mode);

  resumeTimer1();
  resumeTimer3();
  //ADD: blink when ready (after 20-30 sec?)
  
}

void loop() {
  if (mode_change_flag){
    mode_change_flag = false;
    pauseTimer1();
    resetTimer1();
    pauseTimer3();
    signalGeneratorSpeakerOff();
    signalGeneratorLEDOff();
    measurementSamplesSetup(mode);
    resumeTimer1();
    resumeTimer3();
  }
  
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
    String mac;
    getMACAddress(mac, p);
    p.runShellCommand("curl --data \"nokkel=" + mac + "\" --data-urlencode seriedata@" + file + " http://delay.uninett.no/fmaling/dbm.php");
    while(p.running());
    String data = "";
    int i = 0;
    while(p.available() > 0){
      char c = p.read();
      data += c;
      if (i > 0 && c != "[" && c != "]"){
        // Successful upload
        //p.runShellCommand("rm " + file);
        //break;
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

void getMACAddress(String &mac, Process get_mac){
  mac = "";
  get_mac.runShellCommand(F("ifconfig wlan0 | awk '/HWaddr/ {print $NF}'"));
  while (get_mac.available() > 0){
    char c = get_mac.read();
    mac += c;
  }
}

ISR(INT6_vect)
{
  mode_change_flag = true;
  if (mode == VIDEO_MODE){
    mode = SOUND_MODE;
    // Turn off video led
    digitalWrite(videoModeIndicator, LOW);
    // Turn on sound led
    digitalWrite(soundModeIndicator, HIGH);
  }
  else{
    mode = VIDEO_MODE;
    // Turn off sound led
    digitalWrite(soundModeIndicator, LOW);
    // Turn on video led
    digitalWrite(videoModeIndicator, HIGH);
  }
}

