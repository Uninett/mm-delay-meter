#include "wifi.h"

bool getDateStatus(Process p)
/* date_ok.txt contains '0' if the device has not been connected to WiFi since power-up. 
 * When the device has been connected to WiFi, the time and date is set correctly,
 * and date_ok.txt contains '1' */
{
    String result = "";
    p.runShellCommand(F("cat /mnt/sda1/arduino/date_ok.txt"));
    while (p.available() > 0){
        char c = p.read();
        result += c;
    }
    result.trim();
    if (result == "0"){
        return false;
    }
    else{
        return true;
    }
}

void wifiStartup(Process p)
/* Check WiFi configuration and connect at power-up. */
{
    p.runShellCommand(F("/mnt/sda1/arduino/startup.sh"));
    while (p.running());
}

void wifiStatusAndConnectAndUpload(Process p)
/* Runs periodically. Checks WiFi status and reconnects if needed. Uploads new
 * data to a database and marks them as uploaded. */
{
    p.runShellCommand(F("/mnt/sda1/arduino/periodic_wifi_func.sh"));
    while (p.running());
}