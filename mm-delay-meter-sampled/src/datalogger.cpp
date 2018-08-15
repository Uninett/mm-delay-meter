#include <FileIO.h>
#include "datalogger.h"
#include "sensor_samples.h"

// delete files: rm -rf /mnt/sda1/arduino/delay/*

void SDCardSetup()
/* Start the file system for writing to files in SD card through Bridge */
{
    FileSystem.begin();
}

static void SDCardGenerateNewFile(String &name, char mode)
/* Creates a new file with a unique name in /mnt/sda1/arduino/delay directory.
 * '_NU' means 'not uploaded to database'. 
 * Returns the name of the file. */
{	
	Process f;
	f.runShellCommand("mktemp -p /mnt/sd/arduino/delay " + String(mode) + "_NU.XXXXXX");
	while (f.running());
	while (f.available() > 0){
		char c = f.read();
		name += c;
	}
	name.trim();
}


String SDCardSaveData(String start_time, String date, uint8_t measurements, char mode) {
/* Save a measurement series in a file in the SD card, with the format expected by the database. */
{
    // Open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    String filename = "";
    SDCardGenerateNewFile(filename, mode);
    //Serial.print(F("File name: "));
    //Serial.println(filename);
    Log.println("Open file " + filename);
    File data_file = FileSystem.open(filename.c_str(), FILE_APPEND);

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
    	data_string += getSavedMeasurement(i);
    	if ( i < max - 1 ){
    		data_string += "\n";
    	}
    }

    // If the file is available, write to it:
    if (data_file) {
    	//Serial.println(data_string);
        data_file.println(data_string);
        data_file.close();
        Log.println("Write to file " + filename);
    }
    // If the file isn't open, pop up an error:
    else {
        //Serial.println("Error opening " + filename);
        Log.println("Error opening " + filename);
    }
    // Reset to default values, ready for new measurements
    resetNumMeasurementsCompleted();
	resetSavedMeasurements();
	clearMeasuredCompleteFlag();

    return filename;
}


Logger::Logger()
/* Constructor: initializes the name */
{
    log_file = "/mnt/sd/arduino/log.txt";
}

Logger::begin()
/* Start the file system for writing to files via Bridge. Remove any old version of the log */
{
    FileSystem.begin();
    FileSystem.remove(log_file);
}

Logger::print(String input)
/* Print to the log without newline (unused) */
{
    File data_file = FileSystem.open(log_file, FILE_APPEND);
    if (data_file) {
        data_file.print(input);
        data_file.close();
    }
}

Logger::println(String input)
/* Print line to log file, with time stamp */
{
    p.begin("date");
    p.addParameter("+%F %H:%M:%S");
    p.run();
    String date = "";
    while(p.available() > 0) {
        char c = p.read();
        date += c;
    }
    date.trim();

    File data_file = FileSystem.open(log_file, FILE_APPEND);
    if (data_file) {
        data_file.print(date + ": ");
        data_file.println(input);
        data_file.close();
    }
}