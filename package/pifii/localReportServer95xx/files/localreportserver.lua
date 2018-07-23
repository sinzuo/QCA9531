#!/usr/bin/lua
require "luci.model.uci"
local uci = luci.model.uci.cursor()
local pifii = require("pifiilocalreport")
local json = require("luci.json")
local util = require("luci.util")

------debug function start-----
local g_debug_flag = 0--pifii.debug
local g_report_log = "/tmp/localreport.log"
function D(info)
    if g_debug_flag then
        pifii.clean_log(g_report_log)
        if "table" == type(info) then info = json.encode(info) end
        if 0 == g_debug_flag + 0 then
            local str_time = os.date("%Y-%m-%d %H:%M:%S",os.time())
            os.execute("echo \'"..str_time.." -->> "..info.."\' >> "..g_report_log)
        else
            print(info)
        end
    end
end
------debug function end-----

------global variable---------------
--local g_tmp_file = "/tmp/ap_stats.store"
local g_http_headers = {
        ["User-Agent"] = "wifi",
        ["Content-Type"] = "application/json"

} 
------global variable end-----------

function get_assoclist()
    local res = {}
    D("enter get_assoclist()")
    local ifs_str = util.exec([[ls /sys/devices/virtual/net/ | grep ath | tr -s '\n' ' ']])
    local ifs_t = pifii.split(ifs_str," ")
    for _,v in pairs(ifs_t) do
        repeat
            local is_ath = string.find(v,"ath")
            if not is_ath then break end
            --local ssid = util.exec("iwinfo "..v.." info | grep ESSID | awk \'{print$3}\' | tr -d \"\\\"|\\n\" 2>/dev/null")
            --local ssid = util.exec("iwconfig 2>/dev/null | grep "..v.." | awk -F: \'{print $2}\' | tr -d \"\\\"|\\n\"") 
            local ssid = util.exec(string.format([[iwconfig %s 2>/dev/null | grep ESSID | awk -F: '{print$2}' | tr -d "\"|\n"]],v)) 
            ssid = string.gsub(ssid," ","")
            local cmd = "iwinfo "..v.." assoclist | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\"| grep 00:00:00:00:00:00 -v"
            local fd = io.popen(cmd, "r")
            local row = nil
            if fd then
                local line = nil
                while true do
                    local cli = {}
                    line = nil
                    line = fd:read("*l")
                    if not line or #line==0 then break end
                    row=pifii.split(line," ")
                    cli["mac"] = string.upper(row[1])
                    cli["sign"] = row[2]
                    cli["ssid"] = ssid
                    table.insert(res,cli)
                end
            end
            fd:close()
        until true
    end
    return res
end

function get_report_json(json_ver)
    local res = {
        ver = json_ver, 
    }
    local report_params = {
        ap_mac        = pifii.get_ap_mac(),
        ip            = pifii.get_ap_ip(),
        model         = pifii.get_ap_model(),
        firmware      = pifii.get_software_version(),
        cpu           = pifii.get_cpuload(),
        freeram       = pifii.get_freeram(),
        uptime        = pifii.get_sysuptime(),
        config_ver    = pifii.get_confversion(),
        stations      = get_assoclist() --get_client_info3(),
    }
    report_params["incoming"],report_params["outgoing"] = pifii.get_rx_tx()
    res["packet"] = report_params
    return res
end

function do_discovery_ac()
    local socket = require("socket")

    local udp = socket.udp()
    udp:setoption('broadcast', true)
    udp:settimeout(3)
    
    local interval = {2,5,10,20,40,200,0,0,0,0,0} 
    local i = 1
    local ip = nil
    while not ip or "timeout" == ip do
        udp:sendto("Are you AC?\n", "255.255.255.255", 9877)
        _,ip,_ = udp:receivefrom()
        if ip and "timeout" ~= ip then break end
        D("Get AC ip failed")
        os.execute("sleep "..interval[i])
        if 6 < i then
            i = 1 
        else
            i = i + 1
        end  
    end
    return ip 
end

function set_wifi(wt)
    if not wt then
        return false
    end
    uci:load("wireless")
    uci:delete_all("wireless","wifi-iface")
    for _,v in pairs(wt) do
        local t=json.encode(v["type"])
        local k=v["key"] or ""
        local enc=v["encryption"] or ""
        if "" == k or "" == enc then
            k = nil
            enc="none"
        end
        local mft=v["mac_filter_type"]
        if mft and "deny" ~= mft and "allow" ~= mft then
             mft=nil
        end
        if string.find(t,"2g") then
            local options = {
                           device="wifi0",
                           mode="ap",
                           network="lan",
                           ssid=v["ssid"],
                           encryption=enc,
                           key=k,
                           maxsta=v["maxsta"],
                           hidden=v["hidden"],
                           isolate=v["isolate"],
                           portal=v["portal"],
                           commitatf=v["commitatf"],
                           bcn_rate=v["bcn_rate"],  --only 2.4G
                           macfilter=mft,
                           maclist=v["mac_filter_list"],
                       }
            uci:section("wireless","wifi-iface",nil,options)
        end
        local w1=uci:get("wireless","wifi1")
        if w1 and string.find(t,"5g") then
            local options = {
                           device="wifi1",
                           mode="ap",
                           network="lan",
                           ssid=v["ssid"],
                           encryption=enc,
                           key=k,
                           maxsta=v["maxsta"],
                           hidden=v["hidden"],
                           isolate=v["isolate"],
                           portal=v["portal"],
                           commitatf=v["commitatf"],
                           macfilter=mft,
                           maclist=v["mac_filter_list"],
                       }
            uci:section("wireless","wifi-iface",nil,options)
        end
    end
    uci:commit("wireless")
    os.execute("wifi &>/dev/null")
    return true
end

function set_wifibase(wbt)
    if not wbt then
        return false
    end
    
    uci:load("wireless")
    
    local function local_set_wifibase(p,wb)
        if not p or not wb then
            return false
        end
        local conf="wireless"
        local se= "wifi0"
        if "5g" == p then
             se="wifi1"
        end
        
        local ch=wb["channel"]
        local ht=wb["htmode"] --HT20,HT40-,HT80
        local sp=wb["signpower"] --low:40,middle:70,high:100
        local hw=wb["hwmode"] --11b/n/g/ac
        local kick_sta_rssi=wb["outhreshold"]
        local assoc_req_rssi=wb["inthreshold"]
        local auth_rssi=wb["authreshold"]
        
        if ch and "" ~= ch  then
            uci:set(conf,se,"channel",ch)
        end
        if ht and "" ~= ht and ("HT20" == ht or "HT40" == ht or "HT80" == ht)  then
            uci:set(conf,se,"htmode",ht)
        end
        if sp and "" ~= sp  then
            local tp = math.ceil(string.format("%.1f",sp / 100) * 30)
            uci:set(conf,se,"txpower",tp)
        end
        if hw and "" ~= hw then
            uci:set(conf,se,"hwmode",hw)
        end
        if kick_sta_rssi and "" ~= kick_sta_rssi then
            uci:set(conf,se,"KickStaRssiLow",kick_sta_rssi)
        end
        if assoc_req_rssi and "" ~= assoc_req_rssi then
            uci:set(conf,se,"AssocReqRssiThres",assoc_req_rssi)
        end
        if auth_rssi and "" ~= auth_rssi then
            uci:set(conf,se,"AuthRssiThres",auth_rssi)
        end
        
        --2.4G config only
        if "wifi0" == se then
            local lowrate=wb["lowrate"] 
            local auth_count=wb["auth_count"] 
            local probe_count=wb["probe_count"] 

            if lowrate and "" ~= lowrate then
                uci:set(conf,se,"lowrate",lowrate)
            end
            if auth_count and "" ~= auth_count then
                uci:set(conf,se,"auth_count",auth_count)
            end
            if probe_count and "" ~= probe_count then
                uci:set(conf,se,"probe_count",probe_count)
            end
        end
        return true
    end
    
    local flag = false 
    if wbt["2g"] then
        local f2 = local_set_wifibase("2g",wbt["2g"])
        if false == f2 then
            uci:revert("wireless")
            return false
        else
           flag = true
        end 
    end
    
    if wbt["5g"] then
        local f5 = local_set_wifibase("5g",wbt["5g"])
        if false == f5 then
            uci:revert("wireless")
            return false
        else
           flag = true
        end 
    end
    
    if flag then 
        uci:commit("wireless")
        return true
    end
    return false
end

function set_wifi_conf(wt,wbt)
    local f2 = false
    local f5 = false
    if wt then
        D("setwifi")
        f2 = set_wifi(wt)
    end
    if wbt then
        D("setwifibase")
        f5 = set_wifibase(wbt)
    end
    if f2 or f5 then 
        os.execute("wifi &>/dev/null &")
        return true 
    end
    return false 
end

function set_wifidog(wd)
    if not wd then
        return false 
    end  
    local flag = false
    local se = uci:get("wifidog","wifidog")
    if se and "wifidog" == se then
        D("setwifidog")
        uci:load("wifidog")
        if wd["enabled"] then
            uci:set("wifidog",se,"wifidog_enable",wd["enabled"])
            flag = true
        end
        if wd["hostname"] then
            uci:set("wifidog",se,"gateway_hostname",wd["hostname"])
            flag = true
        end
        if wd["httpport"] then
            uci:set("wifidog",se,"gateway_httpport",wd["httpport"])
            flag = true
        end
        if wd["httppath"] then
            uci:set("wifidog",se,"gateway_path",wd["httppath"])
            flag = true
        end
    end
    if flag then
        uci:commit("wifidog")
        os.execute("/sbin/2060reset &>/dev/null &")
    end 
    return flag 
end

function set_probe(pr)
    if not pr then
       return false 
    end
    local flag = false
    local se = uci:get("probe","report")
    if se and se == "report" then
        D("setprobe")
        uci:load("probe")
        if pr["enabled"] then
            uci:set("probe",se,"enable",pr["enabled"])
            flag = true
        end
        if pr["hostname"] then
            uci:set("probe",se,"hostname",pr["hostname"])
            flag = true
        end
        if pr["port"] then
            uci:set("probe",se,"port",pr["port"])
            flag = true
        end
        if pr["interval"] then
            uci:set("probe",se,"interval",pr["interval"])
            flag = true
        end
        if pr["threshold"] then
            uci:set("probe",se,"threshold",pr["threshold"])
            flag = true
        end
    end
    if flag then
        uci:commit("probe")
        os.execute("/etc/init.d/clientprobe restart &")
    end 
    return flag 
end

function do_report_stats()
    --discovery ac
    local ac_ip = nil 
    --discovery ac end
    local report_json_t = nil 
    local resp_code = nil
    local resp_result_str = nil 
    local json_ver = "1.0.0.1"
    local url = nil 
    local resp_ok = false
    local retry_count = 0
    while true do 
        if not ac_ip then
            ac_ip = do_discovery_ac()
            D("AC ip:"..ac_ip)
        end
        repeat
            url="http://"..ac_ip..":81/index.php/index/Trans/ap"
            D(url)
            report_json_t = get_report_json(json_ver)
            D(json.encode(report_json_t))
            --resp_code,resp_result_str = pifii.http_request_post(url,g_http_headers,report_json_t)
            resp_code,resp_result_str = pifii.https_request_post(url,report_json_t)
            if not resp_code or 200 ~= resp_code then
                resp_code = resp_code or "nil"
                D("request failed, response code: "..resp_code)
                retry_count = retry_count + 1
                if 10 < retry_count then
                    ac_ip = nil
                    retry_count = 0
                end
                break
            end
            D(resp_result_str)
            local res_t = json.decode(resp_result_str)
            --D(res_t["ver"]) 
            local jsv = res_t["ver"] 
            if jsv and "1.0.0.1" == jsv then
                if res_t["packet"] and res_t["packet"]["act_sign"] then
                    local ack_t = {
                          ver = jsv,
                          packet = {
                              ap_mac = pifii.get_ap_mac(),
                              act_sign = res_t["packet"]["act_sign"]
                         }
                    }
                    url="http://"..ac_ip..":81/index.php/Home/Trans/ap"
                    pifii.https_request_post(url,ack_t)
                end
                local cmd = nil
                if res_t["packet"] and res_t["packet"]["json_act"] then
                   cmd = res_t["packet"]["json_act"]["cmd"] or ""
                   if "reboot" == cmd then
                        D("reboot")
                        os.execute("reboot &")
                   elseif "reset" == cmd  then
                        os.execute("jffs2reset -y  && reboot &")
                        D("reset")
                   elseif "conf" == cmd  then
                        local conf_ver = res_t["packet"]["json_act"]["config_ver"] or "" 
                        local conf_ver_old = uci.get("localreport","confinfo","conf_version") or "" 
                        if conf_ver ~= conf_ver_old then
                            local flag=false
                            --wifi config
                            local wt = res_t["packet"]["json_act"]["wifi"] 
                            local wbt = res_t["packet"]["json_act"]["wifibase"] 
                            if set_wifi_conf(wt,wbt) then
                                flag = true 
                            end
                            --wifi config end
                            --wifidog config
                            local wifidog = res_t["packet"]["json_act"]["wifidog"] 
                            if wifidog and set_wifidog(wifidog) then
                                flag = true 
                            end
                            --wifidog config end
                            --probe config
                            local probe = res_t["packet"]["json_act"]["probe"] 
                            if probe and set_probe(probe) then
                                flag = true 
                            end
                            --probe config end

                            if flag then
                                uci.set("localreport","confinfo","conf_version",conf_ver) 
                                uci.commit("localreport") 
                            end
                        end
                   elseif "upgrade" == cmd  then
                       D("upgrade")
                       local fw = res_t["packet"]["json_act"]["firmware"]
                       if fw and fw["url"] and "" ~= string.gsub(fw["url"]," ","") and fw["md5"] and "" ~= string.gsub(fw["md5"]," ","") then
                          local dl_code,fw_path = pifii.wget_download(fw["url"],"/tmp",fw["md5"]) 
                          local num = 1
                          while dl_code and 5 == dl_code and 5 > num do
                              num = num + 1
                              D("md5code check failed,try:"..num)
                              dl_code,fw_path = pifii.wget_download(fw["url"],"/tmp",fw["md5"])
                          end
                          if dl_code and 0 == dl_code and fw_path and 0 == os.execute("test -f "..fw_path) then
                              D("firmware download ok")
                              os.execute("/sbin/sysupgrade -c -v "..fw_path.."  >/dev/null 2>&1 &")  
                              os.execute("sleep 180");
                          else
                              D("firmware download failed,return:"..dl_code)
                          end
                       end
                   end 
                end
            end
        until true 
        report_json_t = nil
        os.execute("sleep 16")
    end 
end
----main start-----
do_report_stats()
---main  end---
