/* Datalogger: save to SD card, and print to log file */

#include <FileIO.h>
#include <Process.h>
#include "datalogger.h"
#include "measurement_samples.h"

// delete files: rm -rf /mnt/sda1/arduino/delay/*

void SDCardSetup() {
    // Initialize the Bridge and the Serial
    FileSystem.begin();
}

void SDCardGenerateNewFile(String &name, char mode)
{	
	// Create new file with unique name in /mnt/sda1/arduino/delay directory
	Process f;
	f.runShellCommand("echo $(mktemp -p /mnt/sd/arduino/delay " + String(mode) + ".XXXXXX)");
	while (f.running());
	while (f.available() > 0){
		char c = f.read();
		name += c;
	}
	name.trim();
}


String SDCardSaveData(String start_time, String date, uint8_t measurements, char mode) {
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    String filename = "";
    SDCardGenerateNewFile(filename, mode);
    Serial.print(F("File name: "));
    Serial.println(filename);
    File data_file = FileSystem.open(filename.c_str(), FILE_APPEND);

    Serial.println(F("SD card logging..."));
    String data_string = "";
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
    	if (i >= 15){
	    	// A minute has passed, ~4 sec between each measurement
	    	if (minute < 59){
	    		minute++;
	    	}
	    	else{
	    		hour++;
	    		minute = 0;
	    	}
    	}

    	data_string += String(i);
    	data_string += ",";
    	data_string += date;
    	data_string += ",";
    	data_string += time_string;
    	data_string += ",";
    	data_string += measurementSamplesGetSavedSample(i);
    	if ( i < max - 1 ){
    		data_string += "\n";
    	}
    }

    // if the file is available, write to it:
    if (data_file) {
    	Serial.println(data_string);
        data_file.println(data_string);
        data_file.close();
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


Logger::Logger()
{
    log_file = "/mnt/sd/arduino/log.txt";
}

Logger::begin()
{
    FileSystem.begin();
    FileSystem.remove(log_file);
}

Logger::print(String input)
{
    File data_file = FileSystem.open(log_file, FILE_APPEND);
    if (data_file) {
        data_file.print(input);
        data_file.close();
    }
}

Logger::println(String input)
{
    File data_file = FileSystem.open(log_file, FILE_APPEND);
    if (data_file) {
        data_file.println(input);
        data_file.close();
    }
}