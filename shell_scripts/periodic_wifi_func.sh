#!/bin/sh

BASEDIR="/var/lock"
LOCKFILE="periodic_wifi_func.lock"

(
	# Lock or exit program
	flock -n 9 || exit 1

	wifi_status=$(/mnt/sda1/arduino/wifi_status.sh)
	if [ "$wifi_status" = "0" ]
	then
		echo "Connecting to WiFi."
		/mnt/sda1/arduino/wifi_connect.sh
	else
		echo "Wifi connected. Uploading available files."
		## Write '1' to date_ok.txt
		sed -i -e 's/0/1/g' /mnt/sda1/arduino/date_ok.txt
		/mnt/sda1/arduino/upload.sh

	fi
	if [ "$wifi_status" = "0" ]
	then
		wifi_status=$(/mnt/sda1/arduino/wifi_status.sh)
		if [ "$wifi_status" = "1" ]
		then
			echo "Wifi connected. Uploading available files."
			## Write '1' to date_ok.txt
			sed -i -e 's/0/1/g' /mnt/sda1/arduino/date_ok.txt
			/mnt/sda1/arduino/upload.sh
		else
			echo "Could not connect"
		fi
	fi

) 9>$BASEDIR/$LOCKFILE		# End of lock file construct
