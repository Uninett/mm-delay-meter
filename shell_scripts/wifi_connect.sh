#!/bin/sh

SIGNAL=$(/usr/bin/pretty-wifi-info.lua | grep "Signal" | cut -f2 -d" " | cut -f1 -d"%")
IP=""
SSID=""
CONNECTED=0
IP_STATUS=0
wifilist=""

wifi_status() {
	SIGNAL=$(/usr/bin/pretty-wifi-info.lua | grep "Signal" | cut -f2 -d" " | cut -f1 -d"%")
	MODE=$(/usr/bin/pretty-wifi-info.lua | grep "Mode" | cut -f2 -d" ")
	if [ "$SIGNAL" != "0" -a -n "$SIGNAL" -a "$MODE" = "Client" ]
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

wifi_config_1() {
	blink-start 100					# Start the blue LED blink
	
	hostname Arduino				# Change the current hostname
	uci set system.@system[0].hostname='Arduino'
	uci set arduino.@arduino[0].access_point_wifi_name='YUNArduinoAP'
	
	# This block resets the wifi password
	uci set wireless.@wifi-iface[0].encryption='none'
	uci set wireless.@wifi-iface[0].mode='sta'
	uci set wireless.@wifi-iface[0].ssid='UNINETT_guest'
	uci set wireless.@wifi-iface[0].password=''
	uci set wireless.@wifi-iface[0].identity=''
	uci set wireless.@wifi-iface[0].eap_type=''
	uci set wireless.radio0.channel='auto'
	uci set wireless.radio0.country='NO'
	uci delete network.lan.ipaddr
	uci delete network.lan.netmask
	uci set network.lan.proto='dhcp'
	
	# Change the password
	echo -e "uninett\nuninett" | passwd root
	uci commit
	blink-stop
	wifi
}

wifi_config_2() {
	blink-start 100					# Start the blue LED blink
	
	hostname Arduino				# Change the current hostname
	uci set system.@system[0].hostname='Arduino'
	uci set arduino.@arduino[0].access_point_wifi_name='YUNArduinoAP'
	
	# This block resets the wifi password
	# Makes changes to /etc/config/wireless and /etc/config/network
	uci set wireless.@wifi-iface[0].encryption='mixed-wpa+aes'
	uci set wireless.@wifi-iface[0].mode='sta'
	uci set wireless.@wifi-iface[0].ssid='eduroam'
	uci set wireless.@wifi-iface[0].client_cert='/etc/config/8a1a1133-76ab-498f-a12b-735b77fa7973.crt.pem'
	uci set wireless.@wifi-iface[0].priv_key='/etc/config/8a1a1133-76ab-498f-a12b-735b77fa7973.key.pem'
	uci set wireless.@wifi-iface[0].identity='8a1a1133-76ab-498f-a12b-735b77fa7973@demo.eduroam.no'
	uci set wireless.@wifi-iface[0].eap_type='tls'
	uci set wireless.radio0.channel='auto'
	uci set wireless.radio0.country='NO'
	uci delete network.lan.ipaddr
	uci delete network.lan.netmask
	uci set network.lan.proto='dhcp'
	
	# Change the password of the device
	echo -e "uninett\nuninett" | passwd root
	uci commit
	blink-stop
	wifi

}


wifi_scan() {
	scan=$(iwinfo wlan0 scan | grep ESSID | cut -f2 -d"\"")

	wifilist=$(echo $scan | tr " " "\n")

	#for wifi in $wifilist
	#do
	#	echo "> [$wifi]"
	#done
	
	for wifi in $wifilist
	do
		if [ "$wifi" = "eduroam" ]
		then
			return 1
		fi
	done
	return 0

}


# Check initial status
wifi_status

if [ "$CONNECTED" = "0" ]
then
	# Scan for available networks. Check if eduroam is one of them
	wifi_scan
	EDUROAM_RETURN_CODE=$?

	if [ "$EDUROAM_RETURN_CODE" = "1" ]
	then
		echo "Eduroam available"

		# Configure wifi and wait for connection
		wifi_config_2
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
	else
		echo "Eduroam unavailable"

	fi
	
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
			echo $(date "+%F %H:%M:%S:") "Connected to" $SSID". IP:" $IP >> /mnt/sda1/arduino/log.txt
			break
		fi
	done
	if [ "$IP_STATUS" = "1" ]
	then
		echo "Connected to " $SSID". IP: " $IP
		echo $(date "+%F %H:%M:%S:") "Connected to" $SSID". IP:" $IP >> /mnt/sda1/arduino/log.txt

	fi
	
fi
	