#!/bin/sh

SIGNAL=$(/usr/bin/pretty-wifi-info.lua | grep "Signal" | cut -f2 -d" " | cut -f1 -d"%")
IP=""
SSID=""
CONNECTED=0
IP_STATUS=0

wifi_status() {
	SIGNAL=$(/usr/bin/pretty-wifi-info.lua | grep "Signal" | cut -f2 -d" " | cut -f1 -d"%")
	#echo "Signal: " $SIGNAL
	if [ "$SIGNAL" != "0" -a -n "$SIGNAL" ]
	then
		IP=$(/usr/bin/pretty-wifi-info.lua | grep "IP address" | cut -f2 -d":" | cut -f1 -d"/")
		SSID=$(/usr/bin/pretty-wifi-info.lua | grep "SSID" | cut -f2 -d":")
		CONNECTED=1

		if [ -n "$IP" ]
		then
			IP_STATUS=1
		else
			IP_STATUS=0
		fi

	else
		CONNECTED=0
	fi
}

wifi_config() {
	blink-start 100				# Start the blue LED blink
	
	hostname Arduino				# Change the current hostname
	uci set system.@system[0].hostname='Arduino'
	uci set arduino.@arduino[0].access_point_wifi_name='YUNArduinoAP'
	
	# This block resets the wifi password
	uci set wireless.@wifi-iface[0].encryption='none'
	uci set wireless.@wifi-iface[0].mode='sta'
	uci set wireless.@wifi-iface[0].ssid='UNINETT_guest'
	uci set wireless.@wifi-iface[0].key=''
	uci set wireless.radio0.channel='auto'
	uci set wireless.radio0.country='NO'
	uci delete network.lan.ipaddr
	uci delete network.lan.netmask
	uci set network.lan.proto='dhcp'
	
	# Change the password
	"uninett" | passwd root
	uci commit
	blink-stop
	wifi
}

# Check initial status
wifi_status

if [ "$CONNECTED" = "0" ]
then
	# Configure wifi and wait for connection

	wifi_config
	/bin/sleep 1
	wifi_status
	while [ "$CONNECTED" = "0" ]
	do 
		echo .
		ATTEMPTS=$((ATTEMPTS+1))
		wifi_status
		
		if [ "$ATTEMPTS" -ge "10" ]
		then
			echo "Unable to connect to WiFi."
			ATTEMPTS=0
			break
		fi
		/bin/sleep 1
	done
fi
if [ "$CONNECTED" = "1" ]
then
	# Wait for assigned IP address
	
	while [ "$IP_STATUS" = "0" ]
	do
		wifi_status
		if [ "$IP_status" = 1 ]
		then
			echo "Connected to " $SSID". IP: " $IP
			break
		else
			echo "Connected to " $SSID". No IP yet."
		fi
	done
	if [ "$IP_STATUS" = "1" ]
	then
		echo "Connected to " $SSID". IP: " $IP
	fi
	
fi
	