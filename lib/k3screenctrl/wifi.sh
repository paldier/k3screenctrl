#!/bin/sh
ssid24=`nvram get wl0_ssid`
pd24=`nvram get wl0_wpa_psk`
en24=`nvram get wl0_bss_enabled`
cc24=`k3wl authe_sta_list | wc -l`
ssid5=`nvram get wl1_ssid`
pd5=`nvram get wl1_wpa_psk`
en5=`nvram get wl1_bss_enabled`
cc5=`k3wl authe_sta_list | wc -l`
if [ $(nvram get screen_2G5G_pwd_en) -eq 0 ]; then
pd24="********"
pd5="********"
fi
echo 0
echo $ssid24
echo "$pd24"
echo $en24
echo $cc24
echo $ssid5
echo "$pd5"
echo $en5
echo $cc5
echo ASUS_GUEST
echo 1234567890
echo 0
echo 0

