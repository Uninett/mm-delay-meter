#include "wifi.h"

// void wifiConfig(String yunName, String yunPsw, String wifissid, String wifipsw, String wifiAPname, String countryCode, String encryption)
// {
//     Process p;

//     p.runShellCommand("blink-start 100"); //start the blue blink

//     p.runShellCommand("hostname " + yunName); //change the current hostname
//     p.runShellCommand("uci set system.@system[0].hostname='" + yunName + "'"); //change teh hostname in uci

//     p.runShellCommand("uci set arduino.@arduino[0].access_point_wifi_name='" + wifiAPname + "'");

//     //this block resets the wifi psw
//     p.runShellCommand("uci set wireless.@wifi-iface[0].encryption='" + encryption + "'");
//     p.runShellCommand("uci set wireless.@wifi-iface[0].mode='sta'\n");
//     p.runShellCommand("uci set wireless.@wifi-iface[0].ssid='" + wifissid + "'");
//     p.runShellCommand("uci set wireless.@wifi-iface[0].key='" + wifipsw + "'");
//     p.runShellCommand("uci set wireless.radio0.channel='auto'");
//     p.runShellCommand("uci set wireless.radio0.country='" + countryCode + "'");
//     p.runShellCommand("uci delete network.lan.ipaddr");
//     p.runShellCommand("uci delete network.lan.netmask");
//     p.runShellCommand("uci set network.lan.proto='dhcp'");

//     p.runShellCommand("echo -e \"" + yunPsw + "\n" + yunPsw + "\" | passwd root"); //change the passwors
//     p.runShellCommand("uci commit"); //save the mods done via UCI
//     p.runShellCommand("blink-stop"); //start the blue blink

//     p.runShellCommand("wifi ");
// }

// bool wifiStatus()
// {
//     Process wifiCheck;
//     String resultStr = "";
//     // wifiCheck.runShellCommand(F("/usr/bin/pretty-wifi-info.lua" ));
//     // while (wifiCheck.available() > 0) {
//     //     char c = wifiCheck.read();
//     //     resultStr += c;
//     // }
//     // Serial.println(resultStr);
//     // resultStr = "";
//     // delay(1000);
//     wifiCheck.runShellCommand(F("/usr/bin/pretty-wifi-info.lua | grep \"Signal\" | cut -f2 -d\" \" | cut -f1 -d\"%\"" ));
//     while (wifiCheck.running());
//     while (wifiCheck.available() > 0) {
//         char c = wifiCheck.read();
//         resultStr += c;
//     }
//     delay(1000);
//     Serial.println(resultStr);
//     resultStr.trim();
//     if (resultStr.toInt() != 0){

//         Serial.println("Signal " + resultStr);
//         resultStr = "";
//         wifiCheck.runShellCommand(F("/usr/bin/pretty-wifi-info.lua | grep \"IP address\" | cut -f2 -d\":\" | cut -f1 -d\"/\"" ));
//         while (wifiCheck.running());
//         while (wifiCheck.available() > 0) {
//             char c = wifiCheck.read();
//             resultStr += c;
//         }
//         delay(1000);
//         Serial.println(resultStr);
//         resultStr.trim();
//         //if (resultStr != ""){
//             Serial.println();
//             Serial.print("Connected to UNINETT_guest");      
//             Serial.print(F("\nIP: "));
//             Serial.println(resultStr);
//             return true;
//         // }
//         // else{
//         //     return false;
//         // }
//     }
//     else{
//         return false;
//     }
// }

String wifi = "";

void wifiConnect(Process p)
{
    wifi = "";
    p.runShellCommand("/mnt/sda1/arduino/wifi_connect.sh");
    while (p.available() > 0){
        char c = p.read();
        wifi += c;
    }
    Serial.println(wifi);
}

bool wifiStatus(Process p)
{
    wifi = "";
    p.runShellCommand("/mnt/sda1/arduino/wifi_status.sh");
    while (p.available() > 0){
        char c = p.read();
        wifi += c;
    }
    wifi.trim();
    if (wifi == "0"){
        return false;
    }
    else{
        return true;
    }
}