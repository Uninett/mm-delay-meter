#include <Process.h>

String yunName = "Arduino";
String yunPassword = "uninett";
String networkId[2] = {"DESKTOP-18LJTOF 4069", "UNINETT_guest"};
String networkEnc[2] = {"psk2", "none"};
String networkPsw[2] = {};


void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  Serial.println("...");
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();  // make contact with the linux processor
  digitalWrite(13, HIGH);  // Led on pin 13 turns on when the bridge is ready
  Serial.println("Bridge ready");

}

bool Connected = false;
bool configure = true;
int runs = 0;
int network = 0;

void loop() {
  yunName.trim();
  yunPassword.trim();
  networkId[network].trim();
  networkPsw[network].trim();
  networkEnc[network].trim();
  wifiConfig(yunName, yunPassword, networkId[network], networkPsw[network], "YUNArduinoAP", "NO", networkEnc[network]);
  
  while(!wifiStatus()){
    Serial.print(".");
    runs++;

    if (runs > 10){
      // Stop, try a different wifi
      Serial.print("\nUnable to connect to ");
      Serial.print(networkId[network]);
      Serial.println(". Try a different network.");
      network++;
      runs = 0;
      // If none of the wifis worked, return (and try again later)
      if (network > 1){
        delay(5000);
        network = 0;
      }
      break;
    }
  }

  while(wifiStatus()){
  }  
}

void wifiConfig(String yunName, String yunPsw, String wifissid, String wifipsw, String wifiAPname, String countryCode, String encryption)
{
  Process p;
  
  p.runShellCommand("blink-start 100"); //start the blue blink
  
  p.runShellCommand("hostname " + yunName); //change the current hostname
  p.runShellCommand("uci set system.@system[0].hostname='" + yunName + "'"); //change teh hostname in uci
  
  p.runShellCommand("uci set arduino.@arduino[0].access_point_wifi_name='" + wifiAPname + "'");
  
  //this block resets the wifi psw
  p.runShellCommand("uci set wireless.@wifi-iface[0].encryption='" + encryption + "'");
  p.runShellCommand("uci set wireless.@wifi-iface[0].mode='sta'\n");
  p.runShellCommand("uci set wireless.@wifi-iface[0].ssid='" + wifissid + "'");
  p.runShellCommand("uci set wireless.@wifi-iface[0].key='" + wifipsw + "'");
  p.runShellCommand("uci set wireless.radio0.channel='auto'");
  p.runShellCommand("uci set wireless.radio0.country='" + countryCode + "'");
  p.runShellCommand("uci delete network.lan.ipaddr");
  p.runShellCommand("uci delete network.lan.netmask");
  p.runShellCommand("uci set network.lan.proto='dhcp'");
  
  p.runShellCommand("echo -e \"" + yunPsw + "\n" + yunPsw + "\" | passwd root"); //change the passwors
  p.runShellCommand("uci commit"); //save the mods done via UCI
  p.runShellCommand("blink-stop"); //start the blue blink
  
  p.runShellCommand("wifi ");
}

bool wifiStatus()
{
  Process wifiCheck;
  String resultStr = "";
  wifiCheck.runShellCommand(F("/usr/bin/pretty-wifi-info.lua | grep \"Signal\" | cut -f2 -d\" \" | cut -f1 -d\"%\"" ));
  while (wifiCheck.available() > 0) {
    char c = wifiCheck.read();
    resultStr += c;
  }
  delay(1000);
  resultStr.trim();
  if (resultStr.toInt() != 0){
    resultStr = "";
    wifiCheck.runShellCommand(F("/usr/bin/pretty-wifi-info.lua | grep \"IP address\" | cut -f2 -d\":\" | cut -f1 -d\"/\"" ));
    while (wifiCheck.available() > 0) {
      char c = wifiCheck.read();
      resultStr += c;
    }
    delay(1000);
    resultStr.trim();
    Serial.println();
    Serial.print("Connected to " + networkId[network]);      
    Serial.print(F("\nIP: "));
    Serial.println(resultStr);
    return true;
  }
  else{
    return false;
  }
}

  


