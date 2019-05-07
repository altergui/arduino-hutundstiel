#!/bin/sh

while eval `eval "grep -v ^$ /tmp/tty | tail -n 1"`; do 
    echo > /tmp/tty

    for key in temp1 hum1 temp2 hum2 temp3 hum3 temp4 hum4; do
      [ $key == temp1 ] && value=$temp1
      [ $key == hum1 ] && value=$hum1
      [ $key == temp2 ] && value=$temp2
      [ $key == hum2 ] && value=$hum2
      [ $key == temp3 ] && value=$temp3
      [ $key == hum3 ] && value=$hum3
      [ $key == temp4 ] && value=$temp4
      [ $key == hum4 ] && value=$hum4
	if ! [ -z "$value" ] && ! [ "$value" == "nan" ] ; then
        curl -X POST -H 'X-AIO-Key: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX' \
                     -H 'Content-Type: application/json' \
            https://io.adafruit.com/api/v2/XXXXXXXX/feeds/${key}/data \
                -d '{ "value": '${value}' }' &
        fi
    done

    unset value

    unset temp1
    unset hum1
    unset temp2
    unset hum2
    unset temp3
    unset hum3
    unset temp4
    unset hum4

    sleep 30 # push data only every 30 seconds (some samples are lost)

    while ! grep -q temp4 /tmp/tty ; do
        sleep 1
    done
done



