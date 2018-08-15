/* Run WiFi configuration and status checks manually via Bridge */
#ifndef WIFI_H
#define WIFI_H

#include <Process.h>

bool getDateStatus(Process p);
void wifiStartup(Process p);
void wifiStatusAndConnectAndUpload(Process p);

#endif /*WIFI_H*/