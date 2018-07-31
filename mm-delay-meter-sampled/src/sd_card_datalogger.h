#pragma once

void SDCardSetup();
void SDCardGenerateNewFile(String &name, char mode);
String SDCardLogger(String start_time, String date, uint8_t measurements, char mode);