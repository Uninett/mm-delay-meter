#pragma once
#include <Process.h>

// void wifiConfig(String yunName, String yunPsw, String wifissid, String wifipsw, String wifiAPname, String countryCode, String encryption);
// bool wifiStatus();
void wifiConnect(Process p);
bool wifiStatus(Process p);