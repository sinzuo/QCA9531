#!/bin/sh
pro="/usr/sbin/freecwmpd"

pid=`pgrep $pro`
if [ "" == "$pid" ];then
   /etc/init.d/freecwmpd start &>/dev/null
fi
