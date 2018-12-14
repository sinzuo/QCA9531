#!/usr/bin/lua
local function split(str, delimiter)
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
local function getip(mac)
    return require("luci.util").exec("grep \""..mac:lower().."\" /proc/net/arp | awk \'{print $1}\' | head -n 1 | tr -d \"\\n\"")
end
local function get_assoclist()                                              
    local res = {}                                                    
    local dir = "/sys/devices/virtual/net/"                           
    local ifs_str = require("luci.util").exec("ls "..dir)
    local ifs_t = split(ifs_str," ")
    local wtype="2.4G"
    local i=1
    for _,v in pairs(ifs_t) do                                        
        repeat
	    local is_ath = string.find(v,"ath")
            if not is_ath then break end
            if string.find(v,"ath1") then wtype="5.8G" end
            local ifn = v                                                       
            --local cmd = "iwinfo "..ifn.." assoclist | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\" | grep 00:00:00:00:00:00 -v"
            local cmd = "iwinfo "..ifn.." assoclist | grep -v \'^$\'"
            local fd = io.popen(cmd, "r")                                                      
            local row = nil                                                                     
            local mac = nil                                                                     
            local dbm = nil
            local rx = nil
            local tx = nil
            if fd then                                                                          
                while true do                                                                   
                    local line = nil                                                            
                    line = fd:read("*l")                                                        
                    if not line or #line==0 then break end                                      
                    if 1 == i then
                        row=split(line," ")                                                         
                        mac = string.upper(row[1])                                                                      
                        dbm = row[2]     
                    end
                    if 2 == i then
                        row=split(line," ")                                                         
                        rx = row[2]     
                    end  
                    if 3 == i then
                        row=split(line," ")                                                         
                        tx = row[2]     
                        local cli = {mac=mac,dbm=dbm,wt=wtype,rx=rx,tx=tx}
                        cli["mac"] = mac                                                                      
                        cli["wt"] = wtype
                        res[mac] = cli                                                              
                        i = 0
                    end  
                    i = i + 1
                end                                                                             
            end                                                                                 
            fd:close()                                                                          
        until true
    end                                                                                     
    res["00:00:00:00:00:00"] = nil
    return res                                                                              
end         
local function get_wifi_client()                                                               
    local wifi_list = get_assoclist()                                                       
    local s = require "luci.tools.status"                                                   
    local leases = s.dhcp_leases()                                                          
    local ret = {}                                                                          
    local host_l = {}                                                                       
    local ip_l = {}
    for _, v in pairs(leases) do                                                            
        host_l[v.macaddr:upper()] = v.hostname                                              
        ip_l[v.macaddr:upper()] = v.ipaddr                                             
    end                                                                                     
    local i = 1                                                                             
    for k, v in pairs(wifi_list) do                                                         
        local che = {}                                                                      
        che.mac = k                                                                     
        che.ip = ip_l[k] or getip(k) --arp[k]                                                            
        che.host = host_l[k] or ""                                                      
        che.type = "0"                                                                  
        che.dbm = v["dbm"]
        che.wt = v["wt"]
        che.rx = v["rx"]
        che.tx = v["tx"]
        table.insert(ret,che)                                                           
        i = i + 1;                                                                          
    end                                                                                     
    return ret                                                                              
end        
local ret = get_wifi_client()
if 0 == table.getn(ret) then                                                                          
    print("[]")                                                                                       
else                                                                                                  
    require("luci.json")                                                                              
    local para=luci.json.encode(ret)                                                                  
    print(para)                                                                                       
end      
