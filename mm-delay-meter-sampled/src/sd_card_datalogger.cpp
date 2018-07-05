/* SD card datalogger */

#include <FileIO.h>
#include <Process.h>
#include "sd_card_datalogger.h"
#include "measurement_samples.h"

// delete files: rm -rf /mnt/sda1/arduino/delay/*

void SDCardSetup() {
    // Initialize the Bridge and the Serial
    FileSystem.begin();
}

String SDCardGenerateNewFile()
{	
	// Create new file with unique name in /mnt/sda1/arduino/delay directory
	Process f;
	String name = "";
	f.runShellCommand("echo $(mktemp -t -p /mnt/sd/arduino/delay)");
	while (f.running());
	while (f.available() > 0){
		char c = f.read();
		name += c;
	}
	name.trim();
	return name;
}


String SDCardLogger(String start_time, String date) {
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    String filename = SDCardGenerateNewFile();
    Serial.print("File name: ");
    Serial.println(filename);
    File dataFile = FileSystem.open(filename.c_str(), FILE_APPEND);

    Serial.println("SD card logging...");
    String dataString = "";
    static int i_m;
    double delayMillis;
    static int hour; String s_hour = "";
    static int minute; String s_minute = "";
    Process time;

    // Calculate timestamps
    if (i_m == 0){
    	// Get first timestamp (assume minute accuracy) H:M
    	time.runShellCommand("echo \"" + start_time + "\" | cut -d \":\" -f 1");
    	while (time.available() > 0){
    		char c = time.read();
    		s_hour += c;
    	}
    	hour = s_hour.toInt();
    	time.runShellCommand("echo \"" + start_time + "\" | cut -d \":\" -f 2");
    	while (time.available() > 0){
    		char c = time.read();
    		s_minute += c;
    	}
    	minute = s_minute.toInt();
    }

    if (i_m > 30){
    	// A minute has passed
    	if (minute < 59){
    		minute++;
    	}
    	else{
    		hour++;
    		minute = 0;
    	}
    }
    date.trim();

    for (uint8_t i = 0; i < BUF_SIZE; i++){
    	delayMillis =  measurementSamplesGetSavedSample(i)/1000.0;
    	dataString += String(i_m++);
    	dataString += ",";
    	dataString += date + ",";
    	dataString += String(hour) + ":" + String(minute) + ",";
    	dataString += String(delayMillis);
    	dataString += "\n";
    }

    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.println("error opening " + filename);
    }
    return filename;
}

void SDCardPrintContent()
{
    bool donereading = false;

    String dataString;
    char c;
    String s;

    //open the file for reading:
    File myFile = FileSystem.open("/mnt/sda1/measurements.txt", FILE_READ);
    Serial.println("SD card contents:");

    if (myFile && !donereading) {

    // read from the file until there's nothing else in it:
        while (myFile.available()) {

            c = myFile.read();
            s = s + String(c);
              
        }
        Serial.println(s);
        s = "";
        donereading = true;
    }  

    if (donereading){
        myFile.close();
        Serial.println("END OF THE FILE");
        delay(10000);
    }
    else {
        // if the file didn't open, print an error:
        Serial.println("error opening measurements.txt for reading");
    }
}