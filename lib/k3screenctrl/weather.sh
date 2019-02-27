#!/bin/sh

function get_json_value()
{
  local json=$1
  local key=$2
  if [[ -z "$3" ]]; then
    local num=1
  else
    local num=$3
  fi

  local value=$(echo "${json}" | awk -F"[,:}]" '{for(i=1;i<=NF;i++){if($i~/'${key}'\042/){print $(i+1)}}}' | tr -d '"' | sed -n ${num}p)

  echo ${value}
}
week=$(date +%w)
data=$(date +%Y-%m-%d)
time=$(date +%H:%m)
get_json_value "$(curl -s http://ip.taobao.com/service/getIpInfo.php?ip=myip)" city > /tmp/city
iconv -f gb2312 -t UTF-8 -c /tmp/city
city=`cat /tmp/city`

echo $city
echo 11
echo $data
echo $time
echo 3
echo $week
echo 0
