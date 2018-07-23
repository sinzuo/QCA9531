#!/usr/bin/lua

local util= require("luci.util")
local json= require("luci.json")
local mime= require("mime")
require "luci.model.uci"
local uci = luci.model.uci.cursor()
local g_kick={}

------debug function start-----
local g_debug_flag = 0--pifii.debug
local g_log = "/tmp/aproam.log"
function clean_log(logfile)
    if logfile then
        if 0 == os.execute("test -f "..logfile) then
            local num = util.exec("wc -l "..logfile.." 2>&1 | awk \'{print $1}\' ")
            num = tonumber(num) or 1
            if 5000 <= num + 0 then
               os.execute("echo -n \' \' > "..logfile)
            end
        end
    end
end

function D(info)
    if g_debug_flag then
        clean_log(g_log)
        if "table" == type(info) then info = json.encode(info) end
        if 0 == g_debug_flag + 0 then
            local str_time = os.date("%Y-%m-%d %H:%M:%S",os.time())
            os.execute("echo \'"..str_time.." -->> "..info.."\' >> "..g_log)
        else
            print(info)
        end
    end
end
------debug function end-----

function split(str, delimiter)
    local fs = delimiter
    if not fs or "" == string.gsub(fs," ","") then
                fs="%s"
        end
    if str==nil or str=="" or delimiter==nil then
        return nil
    end

    local result = {}
        for match in string.gmatch((str..delimiter),"[^"..fs.."]+") do
        table.insert(result, match)
    end
    return result
end

function get_scan(thredshold,wt)
    local th = thredshold or -65
    --local cmd = "/usr/bin/getterminal -t "..th.." 2>/dev/null" 
    local cmd = "/usr/bin/getterminal -t "..th.." 2>/dev/null | grep \"0$\""
    if 1 == wt then
        th = thredshold or -70
        cmd = "/usr/bin/getterminal -t "..th.." 2>/dev/null | grep \"1$\""
    end
    local fd = io.popen(cmd, "r")
    local res={}
    if fd then
        local row = nil
        local line = nil
        local mac = nil
        while true do
            line = nil
            line = fd:read("*l")
            if not line or #line==0 then break end
            row=split(line," ")
            mac = string.upper(row[1])
            if nil == res[mac] then
                res[mac] = row[2] --key:mac,value:sign
            else
                if res[mac] + 0 < row[2] + 0 then
                    res[mac] = row[2]
                end
            end
        end
        fd:close()
    end
    return res
end

function get_assoclist()
    uci:load("wireless")
    local th2 = uci:get("wireless","wifi0","KickStaRssiLow")
    local th5 = uci:get("wireless","wifi1","KickStaRssiLow")
    th2 = tonumber(th2)
    th5 = tonumber(th5)
    if not th2 or 0 <= th2 + 0 then th2 = -85 end
    if not th5 or 0 <= th5 + 0 then th5 = -85 end

    local kth2 = uci:get("wireless","wifi0","AuthRssiThres") 
    local kth5 = uci:get("wireless","wifi1","AuthRssiThres") 
    kth2 = tonumber(kth2)
    kth5 = tonumber(kth5)
    if not kth2 or 0 <= kth2 + 0 then kth2 = -95 end
    if not kth5 or 0 <= kth5 + 0 then kth5 = -95 end

    local res = {}
    local count=0
    local dir = "/sys/devices/virtual/net/"
    local ifs_str = util.exec("ls "..dir)
    local ifs_t = split(ifs_str," ")
    local th,kth = nil,nil
    local i=1
    for _,v in pairs(ifs_t) do
        repeat
            local is_ath = string.find(v,"^ath")
            if not is_ath then break end
            is_ath = string.find(v,"ath0") --if 2.4G
            if is_ath then 
               th=th2 
               kth=kth2 
            else
               th=th5
               kth=kth5
            end
            --local cmd = "iwinfo "..v.." assoclist | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\"| grep 00:00:00:00:00:00 -v 2>/dev/null"
            local cmd = "iwinfo "..v.." assoclist 2>/dev/null | grep -v \'^$\'"
            --local ssid =  util.exec("iwinfo "..v.." i | grep ESSID | awk -F\\\" \'{print $2}\' | tr -d \'\n\'")
            local ssid =  util.exec("iwconfig 2>/dev/null | grep "..v.." | awk -F: \'{print $2}\' | tr -d \"\\\"|\\n\"")
            ssid=string.gsub(ssid," ","")
            local fd = io.popen(cmd, "r")
            local row = nil
            local mac = nil
            local sign = nil
            local rx = nil
            local tx = nil
            if fd then
                local line = nil
                while true do
                    line = nil
                    line = fd:read("*l")
                    if not line or #line==0 then break end
                    if 1 == i then
                        row=split(line," ")
                        mac = string.upper(row[1])
                        sign = tonumber(row[2])
                    end
                    if 2 == i then
                        row=split(line," ")
                        rx = tonumber(row[2]) or 0
                    end
                    if 3 == i then
                        row=split(line," ")
                        tx = tonumber(row[2]) or 0
                        if "00:00:00:00:00:00" ~= mac then
                            --[[
                            if sign + 0 <= kth + 0 then --or (string.find(v,"ath0") and tx + 0 < 1) then
                                if not g_kick[mac] then 
                                     g_kick[mac] = 1  
                                elseif 2 <= g_kick[mac] then
                                    os.execute("iwpriv "..v.." kickmac "..mac)
                                    g_kick[mac] = nil
                                    D(v.."-->kickmac:"..mac.." sign:"..sign.." tx:"..tx)
                                else 
                                    g_kick[mac] = g_kick[mac] + 1
                                end
                            else
                            --]]
                                if sign + 0 <= th + 0 then
                                    local cli = {mac=mac,sign=sign,inf=v,ssid=ssid}
                                    table.insert(res,cli)
                                    count=count+1
                                end
                            --[[
                                g_kick[mac] = nil 
                            end
                            --]]
                        end
                        i = 0
                    end
                    i = i + 1
                end
            end
            fd:close()
        until true
    end
    return res,count
end

function get_ap_mac()
    --local cmd="dd bs=1 skip=4 count=6 if=/dev/mtdblock2 2>/dev/null | hexdump -vC | head -n 1 | awk \'{print $2$3$4$5$6$7}\' | tr -d \"\\n\""
    local cmd="ifconfig br-lan | grep HWaddr | awk \'{print $5}\' | tr -d \"\\n\|:\""
    local mac=util.exec(cmd)
    return mac
end

function getsize(t)
    if not t then return 0 end
    local count = 0
    for k,_ in pairs(t) do
        count = count + 1
    end
    return count
end

function main()
    local socket = require("socket")

    local udp = socket.udp()
    udp:setoption('broadcast', true)
    udp:settimeout(3)
    local th=nil
    while(true) do
        local roamlist,count=get_assoclist()
        if 0 < count then
            local res={ap=get_ap_mac(),m="req",rl=roamlist} 
            D("request:"..json.encode(res))
            udp:sendto(mime.b64(json.encode(res)), "255.255.255.255",9866)
        end
        if 30 < getsize(g_kick) then
           g_kick = nil
           g_kick = {}
        end
        os.execute("sleep 10")
    end
    udp:close()
end


main()
