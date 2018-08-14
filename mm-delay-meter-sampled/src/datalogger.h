/* Save measurement series to SD card, and print to log file in SD card */
#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <Process.h>

/* Save measurement series */
void SDCardSetup();
String SDCardSaveData(String start_time, String date, uint8_t measurements, char mode);

/* Print time stamps and messages to log */
class Logger {
private:
	char* log_file;
	Process p;
public:
	Logger();
	begin();
	print(String input);
	println(String input);
};

extern Logger Log;

#endif /*DATALOGGER_H*/