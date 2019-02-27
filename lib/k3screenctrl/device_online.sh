#!/bin/sh
file=/tmp/k3screenctrl/device_online
if [ $# -eq 0 ]; then
    pid_file=/var/run/device_online.sh.pid
    [ -f $pid_file ] && {
        pid=$(cat $pid_file)
        kill -9 $pid >/dev/null 2>&1
    }
    echo $$ > $pid_file
    # device_list=$(grep -w br-lan /proc/net/arp | grep -w 192.168 | awk '{print $1}')
    device_list=$(cat /var/lib/misc/dnsmasq.leases | grep $(nvram get lan_ipaddr | awk -F . '{print $1"."$2"."$3"."}') | awk '{print $3}')
    [ -s $file ] || {
        for device in $device_list
        do
            echo "$device 0" >> $file
        done
    }
    for device in $device_list
    do
        $0 $device >/dev/null 2>&1 &
    done
    sleep 60
    $0 &
    # rm $pid_file
else
    device=$1
    arping -f -q -w 3 -I br-lan $device
    c=$?
    i="$(grep $device $file)"
    if [ -z "$i" ]; then
        echo "$device $c" >> $file
    else
        sed -i 's/'"$i"'/'$device' '$c'/' $file
    fi
fi
