#!/bin/sh

ssid24=`nvram get wl0_ssid`
pd24=`nvram get wl0_wpa_psk`
en24=`nvram get wl0_bss_enabled`
cc24=`wl eth1 authe_sta_list | wc -l`
ssid5=`nvram get wl1_ssid`
pd5=`nvram get wl1_wpa_psk`
en5=`nvram get wl1_bss_enabled`
cc5=`wl eth2 authe_sta_list | wc -l`
echo 0
echo ${ssid24}
echo ${pd24}
echo $en24
echo $cc24
echo ${ssid5}
echo ${pd5}
echo $en5
echo $cc5
echo ASUS-GUEST
echo 123456789
echo 0
echo 0
