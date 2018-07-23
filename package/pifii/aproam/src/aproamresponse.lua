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

function get_maxsta(ssid,ath)                  
    if not ssid or not ath then                                
        return 0                                                      
    end                                                               
    local wt="wifi1"                                                   
    if string.find(ath,"^ath0") then                                    
       wt="wifi0"                                                       
    end                                                                 
    local maxsta=0                                                      
    uci:foreach("wireless","wifi-iface",function(s)                     
                local l_name = s[".name"]                               
                local dev = uci:get("wireless",l_name,"device")         
                local dis = uci:get("wireless",l_name,"disabled") or 0  
                local sd = uci:get("wireless",l_name,"ssid")            
                if 0 == tonumber(dis) and wt == dev and ssid == sd then            
                    local ms = uci:get("wireless",l_name,"maxsta") or 32           
                    maxsta = maxsta + ms                                           
                end                                                                
            end)                                                                   
    return maxsta                                                                  
end        

function get_ssidlist(wt)
    local res={}             
    local al={}
    local ml={}
    local cun=nil --current user number 
    local cmd = "ls /sys/devices/virtual/net/ | grep \"^ath0\" | tr -s \'\n\' \' \'"
    if 1 == wt then                                                            
        cmd = "ls /sys/devices/virtual/net/ | grep \"^ath1\" | tr -s \'\n\' \' \'"  
    end                                                                        
    local str = require("luci.util").exec(cmd)                               
    local t = split(str," ") or {} 
                                              
    for _,v in pairs(t) do                    
        --cmd = "iwinfo "..v.." i 2>/dev/null | grep ESSID | awk -F\\\" \'{print $2}\' | tr -d \'\n\'"
        cmd="iwconfig 2>/dev/null | grep "..v.." | awk -F: \'{print $2}\' | tr -d \"\\\"|\\n\""
        str = require("luci.util").exec(cmd)                                            
        str = string.gsub(str," ","")
        cmd = "iwinfo "..v.." a 2>/dev/null | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\" | wc -l" 
        cun=require("luci.util").exec(cmd) 
        cun = tonumber(cun) 
        if not al[str] then
            al[str] = cun
        else
            al[str] = al[str] + cun 
        end
        ml[str] = get_maxsta(str,v)                                                      
    end                                                                                 
    for k,v in pairs(al) do
        if not ml[k] or (ml[k] and v + 3 < ml[k]) then
            res[k] = 1
        end 
    end
    return res                              
end   

function get_scan(thredshold,wt)
    local th = thredshold or -68
    local cmd = "/usr/bin/getterminal -t "..th.." 2>/dev/null" 
    if 1 == wt then
        th = thredshold or -70
        cmd = "/usr/bin/getterminal -t "..th.." 2>/dev/null | grep \"1$\"" 
    elseif 0 == wt then
        th = thredshold or -65
        cmd = "/usr/bin/getterminal -t "..th.." 2>/dev/null | grep \"0$\"" 
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

function request_pro(rl)
    if not rl then return nil end
    uci:load("wireless")
    local th2 = uci:get("wireless","wifi0","AssocReqRssiThres")
    local th5 = uci:get("wireless","wifi1","AssocReqRssiThres")
    local pd2 = get_scan(th2,0)
    local pd5 = get_scan(th5,1)
    --local pd = get_scan(th2,3)
    local sl2 = get_ssidlist(0)
    local sl5 = get_ssidlist(1)
    local res={}
    local count=0
    local mac=nil
    local sign=nil
    local is_ssid=nil 
    for k,v in pairs(rl) do
       mac=v["mac"] 
       if string.find(v["inf"],"ath0") then
            is_ssid=sl2[v["ssid"]]
            sign=pd2[mac]
       else
            is_ssid=sl5[v["ssid"]]
            sign=pd5[mac]
       end
       --sign=pd[mac]
       if is_ssid and sign and sign + 0 > v["sign"] + 0 then
           v["sign"] = sign 
           table.insert(res,v) 
           count = count + 1
           if string.find(v["inf"],"ath1") then
               os.execute("iwpriv ath0 addmac "..mac.." 2>/dev/null")
           end
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
