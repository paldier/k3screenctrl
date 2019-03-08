#!/bin/sh
WAN_STAT=`nvram get wan0_ipaddr`
WAN6_STAT=`nvram get ipv6_ipaddr`
[ $(nvram get sw_mode) -eq 1 ] && mode=0
[ $(nvram get sw_mode) -eq 1 ] || mode=1
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
echo $IPV4_ADDR
echo $UPLOAD_BPS
echo $DOWNLOAD_BPS
echo 0
echo $mode

