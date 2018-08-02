#include "src/config.h"
#include "src/timer1.h"
#include "src/timer3.h"
#include "src/signal_generator.h"
#include "src/measurement_samples.h"
#include "src/datalogger.h"
#include "src/wifi.h"
#include "src/ui.h"
#include <Process.h>

volatile bool           start_new_series  = false;
volatile bool           running           = false;
volatile uint8_t mode = VIDEO_MODE;
//uint8_t wifi_attempts = 0;
bool    time_status   = false;   // true if the unit has been connected to wifi since power-up
volatile bool    mode_changed  = false;
volatile bool    mode_stopped  = false;
volatile bool    start_measurements = false;
volatile bool    stop_measurements  = false;
//String  networkId     = "UNINETT_guest";
String  date;
String  start_time;
bool first_edge;
volatile unsigned long lastDebounceTime = 0;
volatile unsigned long debounceDelay = 50; 
volatile uint8_t lastButtonState = LOW;
volatile uint8_t buttonState = LOW;
Process p;
Logger Log;


void setup() {
  Serial.begin(9600);
  while(!Serial);

  /* Bridge startup */
  Serial.println(F("Bridge..."));
  Bridge.begin();
  digitalWrite(LED_BUILTIN, HIGH);

  /* Log */
  Log.begin();

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
  Log.println(date);
  
  /* SETUP */
  SDCardSetup();
  signalGeneratorSetup();
  measurementSamplesSetup(mode);
  uiSetup();
  
  interrupts();
}


void loop() {
  if (mode_changed){
    if (!measurementSamplesCheckMeasuredFlag()){
      mode_changed = false;
      setModeLEDs(mode);
      measurementSamplesSetMode(mode);
      measurementSamplesInitialize(mode); 
    }    
  }
  if (start_measurements){
    startMeasurement();
    start_measurements = false;
  }

  if (stop_measurements){
    stopMeasurement();
    stop_measurements = false;
    setAllLEDs(mode);
  }
  
  if (timer1CheckFlag(OVF)){
    if      (mode == VIDEO_MODE) signalGeneratorLEDOn();
    else if (mode == SOUND_MODE) signalGeneratorSpeakerOn();
  }
  if (timer1CheckFlag(COMP)){
    if      (mode == VIDEO_MODE) signalGeneratorLEDOff();
    else if (mode == SOUND_MODE) signalGeneratorSpeakerOff();
    signalGeneratorStatusLEDControl();
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
    clearMeasuredFlag();
    // A full measurement series is complete
    Serial.println(F("Measurement series finished."));
    stopMeasurement();
    
    char mode_char;
    if (mode_stopped){ mode_char = (mode == VIDEO_MODE) ? 'A' : 'V'; }
    else{              mode_char = (mode == VIDEO_MODE) ? 'V' : 'A'; }
    mode_stopped = false;
    
    String file = SDCardSaveData(start_time, date, getNumMeasurementsCompleted(), mode_char);
    
    /* Check WiFi connection */
    if (!wifiStatus(p)){
      /* Not connected. Try again */
      Serial.println(F("WiFi not connected. Trying to reconnect..."));
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
      Serial.println(F("Connected to WiFi. Uploading to database..."));
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
      Serial.println(F("WiFi still not connected. Try again later."));
    }
    setAllLEDs(mode);
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
    start_measurements = true;
  }
  /* STOP */
  else if (running){
    // Save if something can be saved
    if (getNumMeasurementsCompleted() > 0){
      setMeasuredFlag();
    }
    else{
      stop_measurements = true;
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
      } 
      else{ 
        mode = VIDEO_MODE; 
      }
      if (running){
        // Save if something can be saved
        if (getNumMeasurementsCompleted() > 0){
          setMeasuredFlag();
          mode_stopped = true;
        }
        else{
          stop_measurements = true;
          clearMeasuredFlag();
          mode_stopped = false;
        }
      }
      mode_changed = true;
    }
  }
  lastDebounceTime = millis();
  lastButtonState = buttonState;
}

void startMeasurement()
{
  running = true;
  start_new_series = true;
  resetTimer3();
  resetTimer1();
  resumeTimer3();
  resumeTimer1();
  digitalWrite(startIndicator, HIGH);
  digitalWrite(soundModeIndicator, LOW);
  digitalWrite(videoModeIndicator, LOW);
}
void stopMeasurement()
{
  running = false;
  pauseTimer1();
  pauseTimer3();
  signalGeneratorLEDOff();
  signalGeneratorSpeakerOff();
  signalGeneratorStatusLEDControl();
}



