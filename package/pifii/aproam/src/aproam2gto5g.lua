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

function get_5guser()
    uci:load("wireless")
    local dir = "/sys/devices/virtual/net/"
    local ifs_str = util.exec("ls "..dir)
    local ifs_t = split(ifs_str," ")
    local mac=nil
    local res={}
    for _,v in pairs(ifs_t) do
        repeat
            local is_ath = string.find(v,"^ath1")
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
                    mac=string.upper(row[1])
                    table.insert(res,mac)
                end
            end
            fd:close()
        until true
    end
    return res
end

function get_kick2guser()
    uci:load("wireless")
    local p=get_scan(-90,1)
    local dir = "/sys/devices/virtual/net/"
    local ifs_str = util.exec("ls "..dir)
    local ifs_t = split(ifs_str," ")
    local mac=nil
    local res={}
    local flag=true
    for _,v in pairs(ifs_t) do
        repeat
            local is_ath = string.find(v,"^ath0")
            if not is_ath then break end
            table.insert(res,v)
            --[[
            os.execute("iwpriv "..v.." maccmd 2 2>/dev/null")
            flag=true
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
                    mac=string.upper(row[1])
                    if p[mac] then 
                        os.execute("iwpriv "..v.." kickmac "..mac.." 2>/dev/null") 
                        os.execute("iwpriv "..v.." kickmac "..mac.." 2>/dev/null") --exec second
                        os.execute("iwpriv "..v.." addmac "..mac.." 2>/dev/null")
                        D("2gto5g:"..mac)
                    end
                end
            end
            fd:close()
            --]]
        until true
    end
    return res
end

------main-------
local ena = uci:get("aproam","2gto5g","enable")
if tonumber(ena) ~= 1 then return end
while (true) do
    local athlist=get_kick2guser()
    local user5=get_5guser()
    --os.execute("sleep 10")
    --[[
    for _,v in pairs(athlist) do 
        os.execute("iwpriv "..v.." maccmd 3 2>/dev/null") 
        os.execute("iwpriv "..v.." maccmd 2 2>/dev/null") 
    end
    ]]
    for _,v in pairs(athlist) do 
        os.execute("iwpriv "..v.." maccmd 3 2>/dev/null") 
        os.execute("iwpriv "..v.." maccmd 2 2>/dev/null") 
        for _,u in pairs(user5) do 
            os.execute("iwpriv "..v.." addmac "..u.." 2>/dev/null")
        end
    end
    os.execute("sleep 10")
end

