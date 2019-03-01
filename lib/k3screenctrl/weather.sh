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
data=$(date +%Y-%m-%d)
time=$(date +%H:%m)
[ -e "/lib/k3screenctrl/time" ] || echo $(($(date +%s)+7200)) > /lib/k3screenctrl/time
starttime=`cat /lib/k3screenctrl/time`
[ ! -f "/lib/k3screenctrl/http" -o $(date +%s) -ge $starttime ] && json=`curl -s "https://api.seniverse.com/v3/weather/now.json?key=xxxxxxxxxxxxxx&location=ip&language=zh-Hans&unit=c"` && echo $json > /lib/k3screenctrl/http && echo $(($(date +%s)+7200)) > /lib/k3screenctrl/time
#set your key
if [ -n "$(echo $json | grep AP010003)" ]; then
city=北京
temp=11
code=0
else
http=`cat /lib/k3screenctrl/http`
city=`get_json_value "$http" name`
temp=`get_json_value "$http" temperature`
code=`get_json_value "$http" code`
fi
echo $city
echo $temp
echo $data
echo $time
echo $code
echo $week
echo 0

