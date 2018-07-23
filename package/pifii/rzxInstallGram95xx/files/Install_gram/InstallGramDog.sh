#!/bin/sh

PID_FILE=/tmp/InstallGramDog.pid

if [ -f "$PID_FILE" ] ; then
    pid=`cat $PID_FILE`
    kill -9 $pid
fi
echo $$ > $PID_FILE

#get this file dir
#start_c=${0:0:1}
start_c=`echo $0 | sed -r 's/^(.{1})(.+)/\1/g'`
if [ "$start_c" = "/" ] ; then
    dir=${0%/*}
    if [ "$dir" = "" ] ; then
        dir="/"
    fi
elif [ "$start_c" = "." ]; then
#    tmp=${0:2}
tmp=`echo $0 | sed -r 's/^(.{2})(.+)/\2/g'`
    val=`echo $tmp | grep -c "/"`
    if [ $val -eq 0 ] ; then
        tmp=""
    else
        tmp=${tmp%/*}
    fi
    dir=`pwd`"/"${tmp}
else
    val=`echo $0 | grep -c "/"`
    if [ $val -eq 0 ] ; then
        tmp=""
    else
        tmp=${0%/*}
    fi
    dir=`pwd`"/"${tmp}
fi

cd $dir
get_mac()
{
    local mac=`lsap --mac`
#echo $mac | tr '[a-z]' '[A-Z]'
    echo $mac
}

export PATH=$PATH:$dir

while true
do
    die=$( rzx_ps aux  | grep "InstallGram.exe" | grep -v "grep" | wc -l )
    if [ $die -eq 0 ]; then
        ap_mac=`get_mac`
        ./InstallGram.exe  ${ap_mac}  1>/dev/null 2>/dev/null &
    fi
    sleep 30
done
