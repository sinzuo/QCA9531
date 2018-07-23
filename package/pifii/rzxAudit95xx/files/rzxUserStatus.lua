#!/usr/bin/lua

local util= require("luci.util")
local json= require("luci.json")
local mime= require("mime")
require "luci.model.uci"
local uci = luci.model.uci.cursor()
local g_kick={}

------debug function start-----
local g_debug_flag = 0--pifii.debug
local g_log = "/tmp/rzxUserStatus.log"
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

function get_assoclist()
    local res = {}
    local count=0
    local dir = "/sys/devices/virtual/net/"
    local ifs_str = util.exec("ls "..dir)
    local ifs_t = split(ifs_str," ")
    for _,v in pairs(ifs_t) do
        repeat
            local is_ath = string.find(v,"^ath")
            if not is_ath then break end
            D("device:"..v)
            local cmd = "iwinfo "..v.." a 2>/dev/null | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\"| grep 00:00:00:00:00:00 -v"
            --local cmd = "iwinfo "..v.." assoclist 2>/dev/null | grep -v \'^$\'"
            --local ssid =  util.exec("iwinfo "..v.." i | grep ESSID | awk -F\\\" \'{print $2}\' | tr -d \'\n\'")
            local fd = io.popen(cmd, "r")
            local row = nil
            if fd then
                local line = nil
                while true do
                    line = nil
                    line = fd:read("*l")
                    if not line or #line==0 then break end
                    local cli={}
                    row=split(line," ")
                    cli["mac"]=row[1]
                    cli["sign"]=row[2]
                    cli["time"]=(os.time()-((tonumber(row[9]) or 0)/1000))
                    res[row[1]]=cli
                    count=count+1
                end
            end
            fd:close()
        until true
    end
    return res,count
end

function get_dhcp_leases()
    local s = require "luci.tools.status"
    local leases = s.dhcp_leases()
    local res = {}
    local flag=false
    for _, v in pairs(leases) do
        res[v.macaddr:upper()] = v.ipaddr 
        flag=true
    end
    if not flag then
        local cmd="test -f /tmp/ap_mode_dhcp.list && cat /tmp/ap_mode_dhcp.list"
        local fd = io.popen(cmd, "r")
        local row = nil
        local line = nil
        if fd then
            while true do
                line = nil
                line = fd:read("*l")
                if not line or #line==0 then break end
                local cli={}
                row=split(line," ")
                res[row[2]]=row[3]
            end
        end
        fd:close()
    end
    return res
end

function get_his_userlist()
    local res={}
    local cmd="test -f /tmp/rzxuserlist && cat /tmp/rzxuserlist"
    local fd = io.popen(cmd, "r")
    local row = nil
    if fd then
        local line = nil
        while true do
            line = nil
            line = fd:read("*l")
            if not line or #line==0 then break end
            local cli={}
            row=split(line," ")
            cli["ip"]=row[2]
            cli["uptime"]=row[3]
            cli["account"]=row[4]
            cli["auth_type"]=row[5]
            cli["upload"]=row[6]
            cli["download"]=row[7]
            if row[1] then
                res[row[1]]=cli
            end
        end
    end
    fd:close()
    return res
end

function user_online(u) 
    local dir="/tmp/gram/apstatus/on_off_line" 
    os.execute("test -f "..dir.." || mkdir -p "..dir)
    --local fn=dir.."/"..string.gsub(u["ip"],"^%d+.%d+.","").."_"..u["onoff_flag"]..".info"
    local fn=dir.."/"..string.gsub(u["ip"],"^%d+.%d+.","").."_1.info"
    
    local f=io.open(fn,"w")
    f:write(string.format("auth_mode=%s\n",u["auth_mode"] or "0"))
    f:write(string.format("account=%s\n",u["account"] or "")) 
    f:write(string.format("ip_type=%s\n",u["ip_type"] or "4"))      
    f:write(string.format("ip=%s\n",u["ip"])) 
    f:write(string.format("usr_mac=%s\n",u["usr_mac"])) 
    f:write(string.format("onoff_flag=%s\n","1")) 
    f:write(string.format("onoff_time=%s\n",u["onoff_time"] or os.time())) 
    f:write(string.format("nat_port=%s\n",u["nat_port"] or "0")) 
    f:write(string.format("field_strength=%s\n",u["field_strength"] or -45)) 
    f:write(string.format("username=%s\n",u["username"] or "")) 
    f:write(string.format("id_type=%s\n",u["id_type"] or "")) 
    f:write(string.format("id_num=%s\n",u["id_num"] or "")) 
    f:write(string.format("nation=%s\n",u["nation"] or "")) 
    f:write(string.format("card_type=%s\n",u["card_type"] or "")) 
    f:write(string.format("card_num=%s\n",u["card_num"] or "")) 
    f:write(string.format("phone_num=%s\n",u["phone_num"] or "")) 
    f:write(string.format("imei=%s\n",u["imei"] or ""))
    f:write(string.format("terminal_system=%s\n",u["terminal_system"] or "")) 
    f:write(string.format("terminal_brand=%s\n",u["terminal_brand"] or "")) 
    f:write(string.format("terminal_brandtype=%s\n",u["terminal_brandtype"] or "")) 
    f:write(string.format("room=%s\n",u["room"] or ""))
    f:flush()
    f:close() 
end

function user_offline(u)
    local dir="/tmp/gram/apstatus/on_off_line" 
    os.execute("test -f "..dir.." || mkdir -p "..dir)
    local fn=dir.."/"..string.gsub(u["ip"],"^%d+.%d+.","").."_0.info" --10.20_0.info
    
    local f=io.open(fn,"w")
    f:write(string.format("auth_mode=%s\n",u["auth_mode"] or "0"))
    f:write(string.format("account=%s\n",u["account"] or "")) 
    f:write(string.format("ip_type=%s\n",u["ip_type"] or "4"))      
    f:write(string.format("ip=%s\n",u["ip"])) 
    f:write(string.format("usr_mac=%s\n",u["usr_mac"])) 
    f:write(string.format("onoff_flag=%s\n","0")) 
    f:write(string.format("onoff_time=%s\n",u["onoff_time"] or os.time())) 
    f:write(string.format("online_time=%s\n",u["online_time"] or "1000")) 
    f:write(string.format("upload=%s\n",u["upload"] or 100)) 
    f:write(string.format("download=%s\n",u["download"] or 10000)) 
    f:flush()
    f:close()
end

function get_auth_info(addr,port,apmac)
    local socket = require "socket"
    
    local res={}
    if not string.find(addr,"^%d+.%d+.%d+.%d+$") then 
        addr = socket.dns.toip(addr) 
    end
    if not addr then return res end
    port = port or 8888

    local udp = socket.udp()
    udp:settimeout(10)
    udp:setpeername(addr, port)
    
    local s={
        name = "getAuthClient",
        version = "1.0.0",
        serialnumber = apmac,
        commandkey = "getAuthClient"
    }
    
    udp:send(json.encode(s))
    local r=""
    r=udp:receive()
    --local len=string.gsub(string.match(res,"^%d+\{"),"{","")
    local t = json.decode(r)
    if t and t["serialnumber"] and t["serialnumber"] == apmac and t["packet"] and t["packet"]["authclients"] then
        for k,v in pairs(t["packet"]["authclients"]) do 
            if v["mac"] then
               res[v["mac"]] = v
            end
        end
    end
     
    udp:close()
    return res
end

function get_auth_info_by_mac(addr,port,apmac,maclist)
    local socket = require "socket"
    
    local res={}
    if not string.find(addr,"^%d+.%d+.%d+.%d+$") then 
        addr = socket.dns.toip(addr) 
    end
    if not addr then return res end
    port = port or 8888

    local udp = socket.udp()
    udp:settimeout(10)
    udp:setpeername(addr, port)
    
    local s={
        name = "getAuthClientByMac",
        version = "1.0.0",
        serialnumber = apmac,
        commandkey = maclist or "" 
    }
    
    udp:send(json.encode(s))
    local r=""
    r=udp:receive()
    --local len=string.gsub(string.match(res,"^%d+\{"),"{","")
    local t = json.decode(r)
    if t and t["serialnumber"] and t["serialnumber"] == apmac and t["packet"] and t["packet"]["authclients"] and "table" == type(t["packet"]["authclients"]) then
        for k,v in pairs(t["packet"]["authclients"]) do 
            if v["mac"] then
               res[v["mac"]] = v
            end
        end
    end
     
    udp:close()
    return res
end

function get_ap_mac()
    local mac=uci:get_first("freecwmp","device","serial_number")
    if not mac or "" == mac then
        local m=util.exec([[cat /proc/mtd | grep art | awk -F: '{print $1}' | tr -d "\n"]])
        local p="/dev/"..m
        local cmd=[[dd bs=1 skip=0  count=6 if=]]..p..[[ 2>/dev/null | hexdump -vC|grep 00000000|awk '{print toupper($2$3$4$5$6$7)}' | tr -d "\n"]]
        mac=util.exec(cmd)
    end 
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

function get_wifidog_enable()
    uci:load("wifidog")
    return uci:get("wifidog","wifidog","wifidog_enable") or "0"
end

function get_maclist(cl)
    if not cl then return nil end
    local str=nil
    for k,_ in pairs(cl) do
        if not str then
            str=string.gsub(k,":","")
        else
            str=str..","..string.gsub(k,":","")
        end
    end
    return str
end

function main()
    while(true) do
        local cl,count=get_assoclist()
        local dl=get_dhcp_leases()
        local hul=get_his_userlist()
        local isauth = get_wifidog_enable()
        local mac=nil
        D("time:"..os.time())
        os.execute("echo -n \"\" > /tmp/rzxuserlist")
        if "0" == isauth then
            for k,v in pairs(cl)do
                 mac=v["mac"]
                 if not hul[mac] then
                     D("auth:0,up:"..v["mac"]..","..v["sign"]..","..v["time"]..","..dl[mac])
                     local up={}
                     up["auth_mode"] = isauth or "0"
                     up["account"] = ""
                     up["ip_type"] = "4"      
                     up["ip"] = dl[mac]
                     up["usr_mac"] = mac 
                     up["onoff_flag"] = "1"
                     up["onoff_time"] = v["time"] or os.time()
                     up["nat_port"] = "0" 
                     up["field_strength"] = v["sign"] or -45
                     user_online(up) 
                     os.execute("echo \""..v["mac"].." "..dl[mac].." "..v["time"].."\" >> /tmp/rzxuserlist")
                 else
                     os.execute("echo \""..v["mac"].." "..dl[mac].." "..hul[mac]["uptime"].."\" >> /tmp/rzxuserlist")
                     hul[mac]=nil
                 end
            end
            for k,v in pairs(hul)do
                 D("down:"..k) 
                 local dw={}
                 dw["auth_mode"] = 0
                 dw["ip_type"] = 4
                 dw["ip"] = v["ip"]
                 dw["usr_mac"] = k 
                 dw["onoff_flag"] = 0 
                 dw["onoff_time"] = os.time() 
                 dw["online_time"] = os.time() - v["uptime"]
                 dw["upload"] = "" 
                 dw["download"] = "" 
                 user_offline(dw) 
            end

        else --wifidog_enable=1
            local hostname=uci:get_first("freecwmp","acs","hostname") or "121.43.232.251"
            D("auth:1,hostname:"..hostname)
            local maclist=get_maclist(cl)
            local ai=get_auth_info_by_mac(hostname,8888,get_ap_mac(),maclist) --all auth info
            local macshort=""  --
            local au=nil  --a auth user
            for k,v in pairs(cl)do
                 mac=v["mac"]
                 macshort=string.gsub(mac,":","")
                 au=ai[macshort]
                 if au then
                     if not hul[mac] then
                         D("auth:1,up:"..v["mac"]..","..v["sign"]..","..v["time"]..","..au["ip"])
                         local up={}
                         up["account"] = au["username"] or ""
                         if "phone" == au["auth_type"] then
                             up["auth_mode"] = "4" 
                         elseif "weixin" == au["auth_type"] then
                             up["auth_mode"] = "12" 
                             up["account"] = au["unionid"] or au["username"] or ""
                         else
                             up["auth_mode"] = "13" 
                         end
                         up["ip_type"] = "4"      
                         up["ip"] = au["ip"]
                         up["usr_mac"] = mac 
                         up["onoff_flag"] = "1"
                         up["onoff_time"] = os.time() --au["start_time"] or os.time()
                         up["nat_port"] = "0" 
                         up["field_strength"] = v["sign"] or -45
                         user_online(up) 
                         os.execute("echo \""..v["mac"].." "..au["ip"].." "..up["onoff_time"].." "..au["username"]..
                                " "..au["auth_type"].." "..au["output_octets"].." "..au["input_octets"]..
                                "\" >> /tmp/rzxuserlist")
                     else
                         os.execute("echo \""..v["mac"].." "..au["ip"].." "..hul[mac]["uptime"].." "..au["username"]..
                                " "..au["auth_type"].." "..au["output_octets"].." "..au["input_octets"]..
                                "\" >> /tmp/rzxuserlist")
                          hul[mac]=nil
                     end
                 end
            end
            for k,v in pairs(hul)do
                 D("down:"..k) 
                 local dw={}
                 if "phone" == v["auth_type"] then
                     dw["auth_mode"] = "4" 
                 elseif "weixin" == v["auth_type"] then
                     dw["auth_mode"] = "12" 
                 else
                     dw["auth_mode"] = "13" 
                 end
                 dw["account"] = v["account"] or ""
                 dw["ip_type"] = 4
                 dw["ip"] = v["ip"]
                 dw["usr_mac"] = k 
                 dw["onoff_flag"] = 0 
                 dw["onoff_time"] = os.time() 
                 dw["online_time"] = os.time() - v["uptime"]
                 dw["upload"] = v["upload"] or "" 
                 dw["download"] = v["download"] or "" 
                 user_offline(dw) 
            end
        end --if "0" == isauth then end
        os.execute("sleep 16")
    end
end

-------main-------------
local ds=uci:get("rzx","userstatus","disabled")
if not ds or "0" == ds then
    main()
else
    D("disable:1")
end
-------main end-------------
--[[
    local ad=get_auth_info("121.43.232.251",8888,get_ap_mac())    
    for k,v in pairs(ad) do
        print(v["mac"])
    end 
--]]
