#!/bin/sh

# Get MAC address
MAC=$(ifconfig wlan0 | awk '/HWaddr/ {print $NF}')
STATUS=""
CURL_RETURN_CODE=0

# Go through all files in directory /mnt/sda1/arduino/delay
# Audio files: A.XXXXXX - Video files: V.XXXXXX
for file in /mnt/sda1/arduino/delay/A.*; do
	[ -e "$file" ] || continue
	cat $file
	if [ $(wc -l $file | cut -f1 -d" ") -lt "5" ]
	then
		STATUS="AVBRUTT"
	else
		STATUS="MIDLERTIDIG"
	fi
	#curl --data "nokkel=$MAC" --data "status=$STATUS" --data "savetodb=ja&serieno=0" --data "mediatype=Audio" --data "metode=Uninett-fmaler-v1" --data-urlencode seriedata@$file http://delay.uninett.no/fmaling/dbm.php
done
for file in /mnt/sda1/arduino/delay/V.*; do
	[ -e "$file" ] || continue
	cat $file
	if [ $(wc -l $file | cut -f1 -d" ") -lt "5" ]
	then
		STATUS="AVBRUTT"
	else
		STATUS="MIDLERTIDIG"
	fi
	CURL_OUTPUT=$(curl --data "nokkel=$MAC" --data "status=$STATUS" --data "savetodb=ja&serieno=67" --data "mediatype=Video" \
		--data "metode=Uninett-fmaler-v1" --data-urlencode seriedata@$file http://delay.uninett.no/fmaling/dbm.php) \
		|| CURL_RETURN_CODE=$?
	echo $CURL_RETURN_CODE
	echo $CURL_OUTPUT
	TEST=$("$CURL_OUTPUT" | cut -f2 -d">" | cut -f1 -d".")
	echo $TEST
	#if [ "$($CURL_OUTPUT | cut -f2 -d">" | cut -f1 -d".")" = "Insert succeeded" ]
	#then
	#	echo "Insert failed."
	#else
	#	echo "Success!"
	#fi
done

# Try to upload
##curl --data "nokkel=$MAC" --data "savetodb=ja&serieno=57" --data "mediatype=$MEDIATYPE" --data "metode=Uninett-fmaler-v1" --data-urlencode seriedata@$FILE http://delay.uninett.no/fmaling/dbm.php
# Remove from directory if successful