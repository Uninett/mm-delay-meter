/* Save measurement series to SD card, and print to log file in SD card */
#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <Process.h>

/* Save measurement series */
void SDCardSetup();
void SDCardSaveData(String start_time, String date, uint8_t measurements, char mode);

/* Print time stamped messages to log */
class Logger {
private:
	char* log_file;			// Name of the log file
	Process p;				// Process object
public:
	Logger();
	begin();
	print(String input);
	println(String input);
};

extern Logger Log;

#endif /*DATALOGGER_H*/