/* Run WiFi configuration and status checks manually via Bridge */
#ifndef WIFI_H
#define WIFI_H

#include <Process.h>

// void wifiConfig(String yunName, String yunPsw, String wifissid, String wifipsw, String wifiAPname, String countryCode, String encryption);
// bool wifiStatus2();
bool getDateStatus(Process p);
void wifiStartup(Process p);
void wifiStatusAndConnectAndUpload(Process p);

#endif /*WIFI_H*/