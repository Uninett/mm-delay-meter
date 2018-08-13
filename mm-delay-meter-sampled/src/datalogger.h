#pragma once
#include <Process.h>

void SDCardSetup();
String SDCardSaveData(String start_time, String date, uint8_t measurements, char mode);

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