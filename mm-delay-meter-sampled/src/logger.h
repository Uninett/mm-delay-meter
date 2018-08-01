#pragma once

#include <FileIO.h>

class Logger {
public:
	Logger();
	begin();
	print(String input);
	println(String input);
};