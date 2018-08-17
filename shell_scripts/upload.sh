#!/bin/sh

# Get MAC address
MAC=$(ifconfig wlan0 | awk '/HWaddr/ {print $NF}')
STATUS=""
CURL_RETURN_CODE=""

# Go through all files in directory /mnt/sda1/arduino/delay
# Audio files: A_NU.XXXXXX - Video files: V_NU.XXXXXX
for file in /mnt/sda1/arduino/delay/A_NU.*; do
	[ -e "$file" ] || continue
	cat $file
	# Check if the number of lines is less than a full measurement series
	if [ $(wc -l $file | cut -f1 -d" ") -lt "5" ]
	then
		STATUS="AVBRUTT"
	else
		STATUS="MIDLERTIDIG"
	fi
	# Upload to database and get status indicators
	CURL_OUTPUT=$(curl -sSf --data "nokkel=$MAC" --data "status=$STATUS" --data "savetodb=ja&serieno=0" --data "mediatype=Audio" \
		--data "metode=Uninett-fmaler-v1" --data-urlencode seriedata@$file http://delay.uninett.no/fmaling/dbm.php) 
	CURL_RETURN_CODE=$?
	SUBSTRING=$(echo $CURL_OUTPUT | cut -d'>' -f 2 | cut -d'.' -f 1)

	if [ "$SUBSTRING" = "Insert succeeded" -a "$CURL_RETURN_CODE" = "0" ]
	then
		echo "Success!"
		echo $(date "+%F %H:%M:%S:") "UPLOAD - Successfully inserted" $file "to database" >> /mnt/sda1/arduino/log.txt

		new_file=${file/_NU/_U}
		mv $file $new_file	# Rename to indicate the series was uploaded
		touch $new_file		# Update last-modified date
	else
		echo "Insert failed."
		echo $(date "+%F %H:%M:%S:") "UPLOAD - Failed to insert" $file "to database" >> /mnt/sda1/arduino/log.txt

	fi
done

for file in /mnt/sda1/arduino/delay/V_NU.*; do
	[ -e "$file" ] || continue
	if [ $(wc -l $file | cut -f1 -d" ") -lt "5" ]
	then
		STATUS="AVBRUTT"
	else
		STATUS="MIDLERTIDIG"
	fi
	CURL_OUTPUT=$(curl -sSf --data "nokkel=$MAC" --data "status=$STATUS" --data "savetodb=ja&serieno=0" --data "mediatype=Video" \
		--data "metode=Uninett-fmaler-v1" --data-urlencode seriedata@$file http://delay.uninett.no/fmaling/dbm.php) 
	CURL_RETURN_CODE=$?
	SUBSTRING=$(echo $CURL_OUTPUT | cut -d'>' -f 2 | cut -d'.' -f 1)

	if [ "$SUBSTRING" = "Insert succeeded" -a "$CURL_RETURN_CODE" = "0" ]
	then
		echo "Success!"
		echo $(date "+%F %H:%M:%S:") "UPLOAD - Successfully inserted" $file "to database" >> /mnt/sda1/arduino/log.txt
		new_file=${file/_NU/_U}
		mv $file $new_file	# Rename to indicate the series was uploaded
		touch $new_file		# Update last-modified date
	else
		echo "Upload failed."
		echo $(date "+%F %H:%M:%S:") "UPLOAD - Failed to insert" $file "to database" >> /mnt/sda1/arduino/log.txt
	fi
done