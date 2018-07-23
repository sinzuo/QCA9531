#!/bin/sh

local e=$(uci -q get rzx.userstatus.disabled)
local pro="/usr/sbin/rzxUserStatus"
[ "0" == "$e" ] || exit 0
while [ true ];do
        pid=$(pgrep -f "$pro")
        if [ "" = "$pid" ];then
            "$pro" &
        fi
        sleep 20
done
