#!/bin/sh

SIGNAL=$(/usr/bin/pretty-wifi-info.lua | grep "Signal" | cut -f2 -d" " | cut -f1 -d"%")
IP=""
CONNECTED=0
IP_STATUS=0

wifi_status() {
	SIGNAL=$(/usr/bin/pretty-wifi-info.lua | grep "Signal" | cut -f2 -d" " | cut -f1 -d"%")
	MODE=$(/usr/bin/pretty-wifi-info.lua | grep "Mode" | cut -f2 -d" ")
	if [ "$SIGNAL" != "0" -a -n "$SIGNAL" -a "$MODE" = "Client" ]
	then
		IP=$(/usr/bin/pretty-wifi-info.lua | grep "IP address" | cut -f2 -d":" | cut -f1 -d"/")
		CONNECTED=1

		if [ -n "$IP" ]
		then
			IP_STATUS=1
		else
			IP_STATUS=0
		fi

	else
		CONNECTED=0
		IP_STATUS=0
	fi
}

wifi_status
echo $IP_STATUS
