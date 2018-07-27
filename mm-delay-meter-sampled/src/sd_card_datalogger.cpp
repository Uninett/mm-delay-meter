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

void SDCardGenerateNewFile(String &name)
{	
	// Create new file with unique name in /mnt/sda1/arduino/delay directory
	Process f;
	f.runShellCommand("echo $(mktemp -t -p /mnt/sd/arduino/delay)");
	while (f.running());
	while (f.available() > 0){
		char c = f.read();
		name += c;
	}
	name.trim();
}


String SDCardLogger(String start_time, String date, uint8_t measurements) {
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    String filename = "";
    SDCardGenerateNewFile(filename);
    Serial.print("File name: ");
    Serial.println(filename);
    File dataFile = FileSystem.open(filename.c_str(), FILE_APPEND);

    Serial.println("SD card logging...");
    String data_string = "";
    String delayMillis;
    String time_string = "";
    int hour = 0; 
    int minute = 0;
    Process time;

	// Get first timestamp (assume minute accuracy) H:M
	time.runShellCommand("echo \"" + start_time + "\" | cut -d \":\" -f 1");
	while (time.available() > 0){
		char c = time.read();
		time_string += c;
	}
	hour = time_string.toInt();
	time_string = "";
	time.runShellCommand("echo \"" + start_time + "\" | cut -d \":\" -f 2");
	while (time.available() > 0){
		char c = time.read();
		time_string += c;
	}
	minute = time_string.toInt();

    
    date.trim();
    time_string = String(hour) + ":" + String(minute);

    uint8_t max;
    if (measurements == 0) max = BUF_SIZE; else max = measurements;
    for (uint8_t i = 0; i < max; i++){
    	if (i >= 30){
	    	// A minute has passed
	    	if (minute < 59){
	    		minute++;
	    	}
	    	else{
	    		hour++;
	    		minute = 0;
	    	}
    	}

    	delayMillis =  measurementSamplesGetSavedSample(i);
    	data_string += String(i);
    	data_string += ",";
    	data_string += date;
    	data_string += ",";
    	data_string += time_string;
    	data_string += ",";
    	data_string += delayMillis;
    	data_string += "\n";
    	//Serial.println("Date: " + date + "\tTime: " + time_string);
    }

    // if the file is available, write to it:
    if (dataFile) {
    	Serial.println(data_string);
        dataFile.println(data_string);
        dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.println("error opening " + filename);
    }
    // Reset to default values, ready for new measurements
    resetNumMeasurementsCompleted();
	resetSavedMeasurements();
	clearMeasuredFlag();

    return filename;
}
