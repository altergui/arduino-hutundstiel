#!/bin/sh

# NodeMCU hostname is "esp_1313a6"

while eval `eval "wget -O - -q http://esp_1313a6/"`; do 
    for key in temp0 hum0 temp1 hum1 temp2 hum2 temp3 hum3; do
      [ $key == temp0 ] && value=$temp0
      [ $key == hum0 ] && value=$hum0
      [ $key == temp1 ] && value=$temp1
      [ $key == hum1 ] && value=$hum1
      [ $key == temp2 ] && value=$temp2
      [ $key == hum2 ] && value=$hum2
      [ $key == temp3 ] && value=$temp3
      [ $key == hum3 ] && value=$hum3
	if ! [ -z "$value" ] && ! [ "$value" == "nan" ] ; then
        curl -X POST -H 'X-AIO-Key: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX' \
                     -H 'Content-Type: application/json' \
            https://io.adafruit.com/api/v2/XXXXXXXX/feeds/mcu-${key}/data \
                -d '{ "value": '${value}' }' &
        fi
    done

    unset value

    unset temp0
    unset hum0
    unset temp1
    unset hum1
    unset temp2
    unset hum2
    unset temp3
    unset hum3

    sleep 30 # push data only every 30 seconds (some samples are lost)
done
