#include "src/config.h"
#include "src/timer1.h"
#include "src/timer3.h"
#include "src/signal_generator.h"
#include "src/sensor_samples.h"
#include "src/datalogger.h"
#include "src/wifi.h"
#include "src/ui.h"
#include <Process.h>

volatile bool    start_new_series   = false;
volatile bool    running            = false;
volatile bool    mode_changed       = false;
volatile bool    mode_stopped       = false;
volatile bool    start_measurements = false;
volatile bool    stop_measurements  = false;
volatile uint8_t mode               = VIDEO_MODE;
volatile uint8_t lastButtonState    = LOW;
volatile uint8_t buttonState        = LOW;
volatile unsigned long lastDebounceTime = 0;
volatile unsigned long debounceDelay    = 50; 
String date;
String start_time;
bool first_edge;
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
  wifiStartup(p); // Connects wifi, and uploads files to database if wifi connected and any files available
  
  /* SETUP */
  SDCardSetup();
  signalGeneratorSetup();
  sensorSamplesSetup(mode);
  uiSetup();

  Log.println(F("Ready"));
  interrupts();
}


void loop() {
  if (mode_changed){
    if (!checkMeasuredCompleteFlag()){
      mode_changed = false;
      setModeLEDs(mode);
      sensorSamplesSetMode(mode);
      sensorSamplesInitialize(mode); 
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
    bool first_edge = sensorSamplesRisingEdgeDetection(mode, start_new_series);
    if (start_new_series) start_new_series = false;
    if (first_edge){
      // Get start time
      if(getDateStatus(p)){
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
  if (checkMeasuredCompleteFlag()){
    clearMeasuredCompleteFlag();
    // A full measurement series is complete
    Log.println(F("Measurement series finished."));
    stopMeasurement();
    
    char mode_char;
    if (mode_stopped){ mode_char = (mode == VIDEO_MODE) ? 'A' : 'V'; }
    else{              mode_char = (mode == VIDEO_MODE) ? 'V' : 'A'; }
    mode_stopped = false;
    
    SDCardSaveData(start_time, date, getNumMeasurementsCompleted(), mode_char);

    wifiStatusAndConnectAndUpload(p); // Checks WiFi, connects and uploads files if possible
    setAllLEDs(mode);
  }

  /* Reenable interrupts if enough time has passed since previous interrupt, 
     and the mode button was released */
  if (millis() - lastDebounceTime > debounceDelay){
    buttonState = ((PIND & (1 << PD1)) == (1 << PD1));
    if (buttonState == LOW){
      EIMSK |= (1 << INT1);
    }
  }
}

/* Start/stop */
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
      setMeasuredCompleteFlag();
    }
    else{
      stop_measurements = true;
    }
  }
}

/* Change mode */
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
          setMeasuredCompleteFlag();
          mode_stopped = true;
        }
        else{
          stop_measurements = true;
          clearMeasuredCompleteFlag();
          mode_stopped = false;
        }
      }
      mode_changed = true;
    }
  }
  lastDebounceTime = millis();
  lastButtonState = buttonState;
}

static void startMeasurement()
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
static void stopMeasurement()
{
  running = false;
  pauseTimer1();
  pauseTimer3();
  signalGeneratorLEDOff();
  signalGeneratorSpeakerOff();
  signalGeneratorStatusLEDControl();
}



