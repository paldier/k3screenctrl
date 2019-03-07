#!/bin/sh
TEMP_FILE="/tmp/wan_speed_temp"
WAN_STAT=`nvram get wan0_ipaddr`
WAN6_STAT=`nvram get ipv6_ipaddr`
[ "$WAN_STAT" != "0.0.0.0" ] && IPV4_ADDR="$WAN_STAT"
[ "$WAN6_STAT" != "" ] && IPV6_ADDR="$WAN6_STAT"
if [ -n "$IPV4_ADDR" -o -n "$IPV6_ADDR" ]; then
    CONNECTED=1
else
    CONNECTED=0
fi
UPLOAD_BPS=`cat /tmp/k3screenctrl/upspeed`
DOWNLOAD_BPS=`cat /tmp/k3screenctrl/downspeed`
echo $CONNECTED
echo $UPLOAD_BPS
echo $DOWNLOAD_BPS

