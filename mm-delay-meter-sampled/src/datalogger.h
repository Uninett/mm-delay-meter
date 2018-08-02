#pragma once

void SDCardSetup();
void SDCardGenerateNewFile(String &name, char mode);
String SDCardSaveData(String start_time, String date, uint8_t measurements, char mode);

class Logger {
private:
	char* log_file;
public:
	Logger();
	begin();
	print(String input);
	println(String input);
};