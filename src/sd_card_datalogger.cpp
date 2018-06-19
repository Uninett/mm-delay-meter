/* SD card datalogger */

#include <FileIO.h>
#include "sd_card_datalogger.h"
#include "measure_led.h"

void SDCardSetup() {
    // Initialize the Bridge and the Serial
    FileSystem.begin();
    FileSystem.remove("/mnt/sd/measurements.txt");
}


void SDCardLogger(String filename, int num) {
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = FileSystem.open(("/mnt/sd/" + filename).c_str(), FILE_APPEND);

    // make a string that start with a timestamp for assembling the data to log:
    String dataString;
    if (num == 0){
        Process time;
        time.runShellCommand("date");

        // read the output of the command
        while (time.running());
        while (time.available() > 0) {
            char c = time.read();
            dataString += c;
        }
        // if the file is available, write to it:
        if (dataFile) {
            dataFile.print(dataString);
            Serial.print(dataString);
        }
        // if the file isn't open, pop up an error:
        else {
            Serial.println("error opening " + filename);
            return;
        }
    }

    double delay = measureLEDGetDelayMs();
    //double delay = inputCaptureGetDelayMs();
    dataString = String(num);
    dataString += "\t";
    dataString += String(delay);

    

    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.println("error opening " + filename);
    }


}
