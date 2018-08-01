#include "logger.h"
#include <Process.h>

Logger::Logger()
{}

Logger::begin()
{
	FileSystem.begin();
    FileSystem.remove("/mnt/sd/arduino/log.txt");
}

Logger::print(String input)
{
	File data_file = FileSystem.open("/mnt/sd/arduino/log.txt", FILE_APPEND);
	if (data_file) {
        data_file.print(input);
        data_file.close();
    }
}

Logger::println(String input)
{
	File data_file = FileSystem.open("/mnt/sd/arduino/log.txt", FILE_APPEND);
	if (data_file) {
        data_file.println(input);
        data_file.close();
    }
}