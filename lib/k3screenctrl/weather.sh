#!/bin/sh
get_json_value()
{
  local json=$1
  local key=$2
  if [[ -z "$3" ]]; then
    local num=1
  else
    local num=$3
  fi
  local value=$(echo "${json}" | awk -F"[,:}]" '{for(i=1;i<=NF;i++){if($i~/'${key}'"/){print $(i+1)}}}' | tr -d '"' | sed -n ${num}p)
  echo ${value}
}
week=$(date +%w)
data=`cat /lib/k3screenctrl/date`
time=`cat /lib/k3screenctrl/time`
[ -f "/lib/k3screenctrl/starttime" ] || echo $(($(date +%s -d $time)+7200)) > /lib/k3screenctrl/starttime
starttime=`cat /lib/k3screenctrl/starttime`
[ ! -f "/lib/k3screenctrl/http" -o $(date +%s -d $time) -ge $starttime ] && json=`curl -s "https://api.seniverse.com/v3/weather/now.json?key=5fjwjirm6bzk95rx&location=ip&language=zh-Hans&unit=c"` && echo $json > /lib/k3screenctrl/http && echo $(($(date +%s -d $time)+7200)) > /lib/k3screenctrl/starttime
http=`cat /lib/k3screenctrl/http`
city=`get_json_value "$http" name`
temp=`get_json_value "$http" temperature`
code=`get_json_value "$http" code`
echo $city
echo $temp
echo $data
echo $time
echo $code
echo $week
echo 0

