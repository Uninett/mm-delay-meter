/* SD card datalogger */

#include <FileIO.h>
#include "sd_card_datalogger.h"
#include "measure_led.h"

void SDCardSetup() {
  // Initialize the Bridge and the Serial
  FileSystem.begin();
}


void SDCardLogger(String filename) {
  // make a string that start with a timestamp for assembling the data to log:
  String dataString;
  dataString += SDCardGetTimeStamp();
  dataString += " = ";

  float delay = measureLEDGetDelayMs();
  dataString += String(delay);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // The FileSystem card is mounted at the following "/mnt/FileSystema1"
  String filepath = "/mnt/sd/" + filename;
  File dataFile = FileSystem.open(filepath.c_str(), FILE_APPEND);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    SerialUSB.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    SerialUSB.println("error opening " + filename);
  }


}

// This function return a string with the time stamp
String SDCardGetTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.runShellCommand("date");

  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n') {
      result += c;
    }
  }

  return result;
}