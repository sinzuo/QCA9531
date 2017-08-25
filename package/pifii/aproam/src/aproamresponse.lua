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

function get_ap_mac()
    --local cmd="dd bs=1 skip=4 count=6 if=/dev/mtdblock2 2>/dev/null | hexdump -vC | head -n 1 | awk \'{print $2$3$4$5$6$7}\' | tr -d \"\\n\""
    local cmd="ifconfig br-lan | grep HWaddr | awk \'{print $5}\' | tr -d \"\\n\|:\""
    local mac=util.exec(cmd)
    return mac
end

function get_scan(thredshold)
    local th = thredshold or -66
    local cmd = "/usr/bin/getterminal -t "..th.." 2>/dev/null" 
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
    return res,count
end

function request_pro(rl)
    if not rl then return nil end
    uci:load("wireless")
    local th = uci:get("wireless","wifi0","AssocReqRssiThres")
    local pd = get_scan(th)
    if 0 == c then return nil end
    local res={}
    local count=0
    local mac=nil
    local sign=nil
    for k,v in pairs(rl) do
       mac=v["mac"] 
       sign=pd[mac]
       if sign and sign + 0 > v["sign"] + 0 then
           table.insert(res,v) 
           count = count + 1
       end
    end
    return res,count
end

function response_pro(rl)
    for k,v in pairs(rl) do
       os.execute("iwpriv "..v["inf"].." kickmac "..v["mac"].." 2>/dev/null")
       os.execute("iwpriv "..v["inf"].." kickmac "..v["mac"].." 2>/dev/null")--agin execute
    end
end

--
function main()
    local socket = require("socket")
    local port = 9866
    local udp = socket.udp()
    udp:setsockname('*', port)  
    local data, rip , rport
    local running = true
    while running do
        data,rip,rport = udp:receivefrom()
        if data then
            local d=json.decode(mime.unb64(data))
            if d["ap"]  ~= get_ap_mac() then
                if d["m"] and "req" == d["m"] then 
                    local roamlist,c = request_pro(d["rl"]) 
                    if rip and 0 < c then
                        local res={ap=get_ap_mac(),m="res",rl=roamlist} 
                        D("response:"..json.encode(res))
                        udp:sendto(mime.b64(json.encode(res)),rip,port)
                    end 
                elseif d["m"] and "res" == d["m"] then 
                     D("kickmac:"..json.encode(d["rl"]))
                     response_pro(d["rl"]) 
                end
            end
        end
    end
end

main()
