#include "src/config.h"
#include "src/timer1.h"
#include "src/timer3.h"
#include "src/signal_generator.h"
#include "src/measurement_samples.h"
#include "src/sd_card_datalogger.h"
#include "src/wifi.h"
#include <Process.h>

int mode = VIDEO_MODE;
bool mode_change_flag = false;
bool started_measurements = false;
bool running = false;
String date;
String start_time;
String networkId = "UNINETT_guest";
int wifi_attempts = 0;
String filenames[10];
int filenr = 0;
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
  pinMode(startIndicator, OUTPUT);
  pinMode(videoModeIndicator, OUTPUT);
  pinMode(soundModeIndicator, OUTPUT);
  pinMode(modeSelectPin, INPUT);
  pinMode(startPin, INPUT);
  digitalWrite(startIndicator, LOW);
  digitalWrite(videoModeIndicator, HIGH);
  digitalWrite(soundModeIndicator, LOW);
  // Configure external interrupt INT6, rising edge
  EICRB |= (1 << ISC61) | (1 << ISC60);
  EIMSK |= (1 << INT6);
  // Configure external interrupt INT0, rising edge
  EICRA |= (1 << ISC01) | (1 << ISC00);
  EIMSK |= (1 << INT0);
  interrupts();

  SDCardSetup();
  signalGeneratorSetup(mode);
  measurementSamplesSetup(mode);

  /* WiFi */
//  wifiConfig("Arduino", "uninett", networkId, "", "YUNArduinoAP", "NO", "none");
//  if(!wifiStatus()){
//    wifiConfig("Arduino", "uninett", networkId, "", "YUNArduinoAP", "NO", "none");
//    while(!wifiStatus()){
//      Serial.print(".");
//      wifi_attempts++;
//      if (wifi_attempts >= 10){
//        Serial.print("\nUnable to connect to ");
//        Serial.print(networkId);
//        Serial.println(".");
//        wifi_attempts = 0;
//        break;
//      }
//    }
//  }
//  Process wifiCheck;
//  String resultStr = "";
//  wifiCheck.runShellCommand(F("/usr/bin/pretty-wifi-info.lua | grep \"IP address\" | cut -f2 -d\":\" | cut -f1 -d\"/\"" ));
//  while (wifiCheck.running());
//  while (wifiCheck.available() > 0) {
//     char c = wifiCheck.read();
//     resultStr += c;
//  }
//  resultStr.trim();
//  Serial.print("IP: ");
//  Serial.println(resultStr);
//  
//  while (resultStr == ""){
//    resultStr = "";
//    wifiCheck.runShellCommand(F("/usr/bin/pretty-wifi-info.lua | grep \"IP address\" | cut -f2 -d\":\" | cut -f1 -d\"/\"" ));
//    while (wifiCheck.running());
//    while (wifiCheck.available() > 0) {
//       char c = wifiCheck.read();
//       resultStr += c;
//    }
//    resultStr.trim();
//    Serial.print("IP: ");
//    Serial.println(resultStr);
//    delay(2000);
//  }
  
  //delay(60000);
  /* Test shell script */
  wifiConnect(p);


  //resumeTimer1();
  //resumeTimer3();
  //ADD: blink when ready (after 20-30 sec?)
  
}

void loop() {
  if (mode_change_flag){
    mode_change_flag = false;
    pauseTimer1();
    pauseTimer3();
    signalGeneratorSpeakerOff();
    signalGeneratorLEDOff();
    measurementSamplesSetup(mode);
    //resumeTimer1();
    //resumeTimer3();
    running = false;
    digitalWrite(startIndicator, LOW);
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
    if (first_edge){
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
    }
  }
  if (measurementSamplesCheckMeasuredFlag()){
    // A full measurement series is complete
    pauseTimer1();
    pauseTimer3();
    signalGeneratorLEDOff();
    if (mode == SOUND_MODE){
      signalGeneratorSpeakerOff();
    }
    String file = SDCardLogger(start_time, date);
    if (filenr < 10){
      filenames[filenr++] = file;
    }
    
    /* Check WiFi connection */
    Serial.println("Measurement series finished.");
    
    if (!wifiStatus(p)){
      /* Not connected. Try again */
      Serial.println("WiFi not connected. Trying to reconnect...");
      wifiConnect(p);      
    }
    
    if (wifiStatus(p)){
      /* Connected. Upload to database */
      Serial.println("Connected to WiFi. Uploading to database...");
//      String mac;
//      getMACAddress(mac, p);
//      for (int nr = 0; nr < filenr; nr++){
//        //p.runShellCommand("curl --data \"nokkel=" + mac + "\" --data-urlencode seriedata@" + filenames[nr] + " http://delay.uninett.no/fmaling/dbm.php");
//        //while(p.running());
//        String data = "";
//        int i = 0;
//        while(p.available() > 0){
//          char c = p.read();
//          data += c;
//          if (i > 0 && c != "[" && c != "]"){
//            // Successful upload
//            Serial.println("Uploaded file " + filenames[nr]);
//            //p.runShellCommand("rm /mnt/sd/arduino/delay/" + filenames[nr]);
//            break;
//          }
//          i++;
//        }
//        filenames[nr] = "";
//        Serial.println(data);
//      }
//      filenr = 0; // Assumed all uploads were successful
      
    }
    else{
      Serial.println("WiFi still not connected. Try again later.");
    }
    
//    if(!wifiStatus()){
//      wifiConfig("Arduino", "uninett", networkId, "", "YUNArduinoAP", "NO", "none");
//      while(!wifiStatus()){
//        Serial.print(".");
//        wifi_attempts++;
//    
//        if (wifi_attempts >= 10){
//          Serial.print("\nUnable to connect to ");
//          Serial.print(networkId);
//          Serial.println(".");
//          wifi_attempts = 0;
//          break;
//        }
//      }
//    }
//    if(wifiStatus()){
//      String mac;
//      getMACAddress(mac, p);
//      for (int nr = 0; nr < filenr; nr++){
//        //p.runShellCommand("curl --data \"nokkel=" + mac + "\" --data-urlencode seriedata@" + filenames[nr] + " http://delay.uninett.no/fmaling/dbm.php");
//        //while(p.running());
//        String data = "";
//        int i = 0;
//        while(p.available() > 0){
//          char c = p.read();
//          data += c;
//          if (i > 0 && c != "[" && c != "]"){
//            // Successful upload
//            Serial.println("Uploaded file " + filenames[nr]);
//            //p.runShellCommand("rm /mnt/sd/arduino/delay/" + filenames[nr]);
//            break;
//          }
//          i++;
//        }
//        filenames[nr] = "";
//        Serial.println(data);
//      }
//      filenr = 0; // Assumed all uploads were successful
//    }
//    else{
//      // Try to connect
//      // Repeat?
//    }
    
    running = false;
    digitalWrite(startIndicator, LOW);
    //while(true);    // TEMPORARY: Stop when a measurement series is complete
    //SDCardPrintContent();
    //resumeTimer1();
    //resumeTimer3();
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

ISR(INT0_vect)
{
  // Begin/stop measurement series
  if (!running){
    running = true;
    resetTimer3();
    resetTimer1();
    resumeTimer3();
    resumeTimer1();
    digitalWrite(startIndicator, HIGH);
  }
  else if (running){
    running = false;
    pauseTimer3();
    pauseTimer1();
    signalGeneratorLEDOff();
    signalGeneratorSpeakerOff();
    digitalWrite(startIndicator, LOW);
  }
}

