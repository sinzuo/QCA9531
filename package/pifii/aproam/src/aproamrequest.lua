#!/usr/bin/lua

local util= require("luci.util")
local json= require("luci.json")
local mime= require("mime")
require "luci.model.uci"
local uci = luci.model.uci.cursor()

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

function get_assoclist(th)
    th = tonumber(th)
    if not th or 0 <= th + 0 then th = -85 end
    local kth = uci:get("wireless","wifi0","AuthRssiThres") 
    if not kth or 0 <= kth + 0 then kth = -90 end
    local res = {}
    local count=0
    local dir = "/sys/devices/virtual/net/"
    local ifs_str = util.exec("ls "..dir)
    local ifs_t = split(ifs_str," ")
    for _,v in pairs(ifs_t) do
        repeat
            local is_ath = string.find(v,"ath")
            if not is_ath then break end
            local cmd = "iwinfo "..v.." assoclist | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\"| grep 00:00:00:00:00:00 -v 2>/dev/null"
            local fd = io.popen(cmd, "r")
            local row = nil
            if fd then
                local line = nil
                while true do
                    line = nil
                    line = fd:read("*l")
                    if not line or #line==0 then break end
                    row=split(line," ")
                    if row[2] + 0 < kth + 0 then
                        os.execute("iwpriv "..v.." kickmac "..string.upper(row[1]))
                        D("kickmac:"..row[1].." sign:"..row[2])
                    else
                        if row[2] + 0 < th + 0 then
                            local cli = {}
                            cli["inf"] = v
                            cli["mac"] = string.upper(row[1])
                            cli["sign"] = row[2]
                            table.insert(res,cli)
                            count=count+1
                        end
                    end
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

function main()
    local socket = require("socket")

    local udp = socket.udp()
    udp:setoption('broadcast', true)
    udp:settimeout(3)
    local th=nil
    while(true) do
        uci:load("wireless")
        th = uci:get("wireless","wifi0","KickStaRssiLow")
        local roamlist,count=get_assoclist(th)
        if 0 < count then
            local res={ap=get_ap_mac(),m="req",rl=roamlist} 
            D("request:"..json.encode(res))
            udp:sendto(mime.b64(json.encode(res)), "255.255.255.255",9866)
        end
        os.execute("sleep 10")
    end
    udp:close()
end


main()
