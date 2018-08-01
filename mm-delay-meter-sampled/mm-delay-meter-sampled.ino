#include "src/config.h"
#include "src/timer1.h"
#include "src/timer3.h"
#include "src/signal_generator.h"
#include "src/measurement_samples.h"
#include "src/sd_card_datalogger.h"
#include "src/wifi.h"
#include <Process.h>

volatile bool           mode_button       = false;
volatile bool           prev_mode_button  = false;
volatile bool           start_new_series  = false;
volatile bool           mode_change_flag  = false;
volatile bool           running           = false;
volatile uint8_t mode = VIDEO_MODE;
uint8_t wifi_attempts = 0;
bool    time_status   = false;   // true if the unit has been connected to wifi since power-up
volatile bool    mode_changed  = false;
volatile bool    mode_stopped  = false;
String  networkId     = "UNINETT_guest";
String  date;
String  start_time;
bool first_edge;
Process p;
volatile unsigned long lastDebounceTime = 0;
volatile unsigned long debounceDelay = 50; 
volatile uint8_t lastButtonState = LOW;
volatile uint8_t buttonState = LOW;



void setup() {
  Serial.begin(9600);
  while(!Serial);

  /* Bridge startup */
  Serial.println("Bridge...");
  Bridge.begin();
  Serial.println("Bridge setup complete");
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
  // Configure external interrupt INT0, rising edge, D3
  EICRA |= (1 << ISC01) | (1 << ISC00);
  //EICRA &= ~(1 << ISC01);
  //EICRA &= ~(1 << ISC00);
  EIMSK |= (1 << INT0);
  // Configure external interrupt INT6, rising edge, D7
//  EICRB |= (1 << ISC61) | (1 << ISC60); 
//  EICRB |= (1 << ISC61);
//  EICRB &= ~(1 << ISC60);
//  EIMSK |= (1 << INT6); 
  // Configure external interrupt INT1, edge, D2
  //EICRA |= (1 << ISC11) | (1 << ISC10); 
  EICRA &= ~(1 << ISC11);
  EICRA |= (1 << ISC10);
  EIMSK |= (1 << INT1); 
  
  interrupts();

  SDCardSetup();
  signalGeneratorSetup();
  measurementSamplesSetup(mode);

  /* WiFi */
  p.begin("date");
  p.addParameter("+%F %H:%M:%S");
  p.run();
  date = "";
  while(p.available() > 0) {
    char c = p.read();
    date += c;
  }
  Serial.println(date);
  
//  wifiConfig("Arduino", "uninett", networkId, "", "YUNArduinoAP", "NO", "none");
//  if(!wifiStatus2()){
//    wifiConfig("Arduino", "uninett", networkId, "", "YUNArduinoAP", "NO", "none");
//    while(!wifiStatus2()){
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

  wifiConnect(p);
  if (wifiStatus(p)){
    time_status = true;
  }
  p.begin("date");
  p.addParameter("+%F %H:%M:%S");
  p.run();
  date = "";
  while(p.available() > 0) {
    char c = p.read();
    date += c;
  }
  Serial.println(date);

  //ADD: blink when ready (after 20-30 sec?)

}


void loop() {
  if (mode_changed){
    mode_changed = false;
    measurementSamplesSetMode(mode);
    measurementSamplesInitialize(mode); 
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
    bool first_edge = measurementSamplesRisingEdgeDetection(mode, start_new_series);
    if (start_new_series) start_new_series = false;
    if (first_edge){
      // Get start time
      if (time_status){
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
      else{
        date = "1970-01-01";
        start_time = "00:00";
      }
    }
  }
  if (measurementSamplesCheckMeasuredFlag()){
    // A full measurement series is complete
    Serial.println("Measurement series finished.");
    pauseTimer1();
    pauseTimer3();
    signalGeneratorLEDOff();
    if (mode == SOUND_MODE){
      signalGeneratorSpeakerOff();
    }
    running = false;
    digitalWrite(startIndicator, LOW);
    
    char mode_char;
    if (mode_stopped){ mode_char = (mode == VIDEO_MODE) ? 'A' : 'V'; }
    else{           mode_char = (mode == VIDEO_MODE) ? 'V' : 'A'; }
    mode_stopped = false;
    
    String file = SDCardLogger(start_time, date, getNumMeasurementsCompleted(), mode_char);
    
    /* Check WiFi connection */
    if (!wifiStatus(p)){
      /* Not connected. Try again */
      Serial.println("WiFi not connected. Trying to reconnect...");
      wifiConnect(p);      
    }
    
    if (wifiStatus(p)){
      /* Connected. Upload to database */
      time_status = true;
      // Does it sync automatically or do I need to attempt to get date?
      p.begin("date");
      p.addParameter("+%F %H:%M:%S");
      p.run();
      date = "";
      while(p.available() > 0) {
        char c = p.read();
        date += c;
      }
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
  }

  /* Reenable interrupts if enough time has passed since previous interrupt, 
     and the button was released */
  if (millis() - lastDebounceTime > debounceDelay){
    buttonState = ((PIND & (1 << PD1)) == (1 << PD1));
    if (buttonState == LOW){
      EIMSK |= (1 << INT1);
    }
  }
}




ISR(INT0_vect)
{
  /* START */
  if (!running){
    running = true;
    start_new_series = true;
    resetTimer3();
    resetTimer1();
    resumeTimer3();
    resumeTimer1();
    digitalWrite(startIndicator, HIGH);
  }
  /* STOP */
  else if (running){
    running = false;
    pauseTimer3();
    pauseTimer1();
    signalGeneratorLEDOff();
    signalGeneratorSpeakerOff();
    digitalWrite(startIndicator, LOW);
    // Save if something can be saved
    if (getNumMeasurementsCompleted() > 0){
      setMeasuredFlag();
      Serial.print("Num measurements: ");
      Serial.println(getNumMeasurementsCompleted());
    }
  }
}

ISR(INT1_vect) 
{ 
  // Disable interrupts from this pin
  EIMSK &= ~(1 << INT1);
  // Digital pin 2 = (SDA/INT1) PD1
  buttonState = ((PIND & (1 << PD1)) == (1 << PD1));
  
  // If some time has passed since the last interrupt
  if (millis() - lastDebounceTime > debounceDelay){
    // Check if the button was pressed, not released
    if (buttonState == HIGH && lastButtonState == LOW){
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
      if (running){
        Serial.println("Stopping");
        running = false;
        pauseTimer1();
        pauseTimer3();
        signalGeneratorSpeakerOff();
        signalGeneratorLEDOff();
        digitalWrite(startIndicator, LOW);
        // Save if something can be saved
        if (getNumMeasurementsCompleted() > 0){
          Serial.print("Num measurements: ");
          Serial.println(getNumMeasurementsCompleted());
          setMeasuredFlag();
          mode_stopped = true;
        }
        else{
          clearMeasuredFlag(); //???
          mode_stopped = false;
        }
      }
      mode_changed = true;
    }
  }
  lastDebounceTime = millis();
  lastButtonState = buttonState;
}

