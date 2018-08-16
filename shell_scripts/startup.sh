#!/bin/sh

echo 0 > /mnt/sda1/arduino/date_ok.txt
/mnt/sda1/arduino/wifi_connect.sh

wifi_status=$(/mnt/sda1/arduino/wifi_status.sh)
if [ "$wifi_status" = "1" ]
then
	sed -i -e 's/0/1/g' /mnt/sda1/arduino/date_ok.txt
	/mnt/sda1/arduino/upload.sh

fi