local io = require("io") 
local json = require("luci.json")
local util = require("luci.util")
local http = require("socket.http") 
local ltn12 = require("ltn12")

local _M = {}

--log 
_M.log_file = nil --log file"
_M.log_flag = 0 -- nil:don't print log,0:print log to file,1: print log to stdout 
_M.log_max = 5000 --max line num

function _M.log_info(info)
    if _M.log_flag and _M.log_file then
        _M.clean_log()
        if not info then info = " " end 
        if "table" == type(info) then info = json.encode(info) end
        local str_time = os.date("%Y-%m-%d %H:%M:%S",os.time())
        if 0 == _M.log_flag + 0 then
            local lf = io.open(_M.log_file, "a+")
            io.output(lf) 
            io.write(string.format([[%s -->> %s%s]],str_time,string.gsub(info,"\n",""),"\n"))
            io.close(lf)
        else
            print(str_time.." -->> "..info)
        end 
    end 
end

function _M.clean_log()
    if _M.log_file then
        if 0 == os.execute("test -f ".._M.log_file) then
            local function exec(command)
                local pp   = io.popen(command)
                local data = pp:read("*a")
                pp:close()
                return data
            end
            local num = exec("wc -l ".._M.log_file.." 2>&1 | awk \'{print $1}\' ")
            num = tonumber(num) or 1
            if _M.log_max <= num + 0 then
               os.execute("echo -n \' \' > ".._M.log_file)
            end 
        end 
    end 
end
--log end

function _M.exec(command)
    local pp   = io.popen(command)
    local data = pp:read("*a")
    pp:close()
    return data
end

function _M.sleep(n)
   require("socket").select(nil, nil, n)
end

function _M.split(str, delimiter)
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

function _M.check_ip(ip)
    if not ip or "" == ip then return nil end
    local ipt = _M.split(ip,"\.")
    local num = nil
    if 4 ~= #ipt then return nil end
    num = tonumber(ipt[1]); if not num or 0 > num or 255 < num then return nil end
    num = tonumber(ipt[2]); if not num or 0 > num or 255 < num then return nil end
    num = tonumber(ipt[3]); if not num or 0 > num or 255 < num then return nil end
    num = tonumber(ipt[4]); if not num or 0 > num or 255 < num then return nil end
    return ipt
end

function _M.ubus_inface_status(inf,param,index) 
    local arg_t = {inf=inf,param=param}
    if arg_t and arg_t["inf"] and arg_t["param"] then
        local ifa = util.ubus(string.format("network.interface.%s",arg_t["inf"]),"status")
        if ifa then
            local p = arg_t["param"]
            if "ipv4-address" == p then
                if ifa[p] and ifa[p][1] then
                    local res = ifa[p][1]["address"] or ""
                    return res
                end 
            elseif "mask" == p then
                if ifa["ipv4-address"] and ifa["ipv4-address"][1] then
                    local res = ifa[ipv4-address][1]["mask"] or ""
                    return res
                end 
            elseif "dns-server" == p then
                if index and ifa[p] and ifa[p][index] then
                    return ifa[p][index]
                end 
            elseif "nexthop" == p then
                if ifa["route"] and ifa["route"][2] then
                    local res = ifa["route"][2][p] or ""
                    return res
                elseif ifa["route"] and ifa["route"][1] then
                    local res = ifa["route"][1][p] or ""
                    return res
                end
            elseif "mac" == p then
                local cmd = string.format([[ifconfig %s | grep HWaddr | awk '{print $5}' | tr -d '\n']],ifa["device"])
                return util.exec(cmd)
            elseif "netmask" == p then
                local cmd = string.format([[ifconfig %s | grep Mask | awk -F: '{print $4}' | tr -d '\n']],ifa["device"])
                return util.exec(cmd)
            elseif "broadcast" == p then
                local cmd = string.format([[ifconfig %s | grep Bcast | awk '{print $3}' | tr -d 'Bcast:|\n']],ifa["device"])
                return util.exec(cmd)
            else
                if ifa[p] and "table" ~= type(ifa[p]) then
                    return ifa[p]
                end 
            end 
        end 
    end
end

function _M.get_up_network_info(param)
    local value = _M.ubus_inface_status("lan",param)
    local lproto = _M.ubus_inface_status("lan","proto")
    if lproto and "static" == lproto then
        value = _M.ubus_inface_status("wan",param)
 
        local bup = _M.ubus_inface_status("bridge","up")
        local bip = _M.ubus_inface_status("bridge","ipv4-address")
        if true == bup and _M.check_ip(bip) then
            value = _M.ubus_inface_status("bridge",param)
        end
    end 
    return value 
end

function _M.get_ifname()
    return _M.get_up_network_info("device")
end

function _M.get_mac()
    local param = "mac"
    local value = _M.ubus_inface_status("lan",param)
    local lproto = _M.ubus_inface_status("lan","proto")
    if lproto and "static" == lproto then
        value = _M.ubus_inface_status("wan",param)
        local bup = _M.ubus_inface_status("bridge","up")
        local bip = _M.ubus_inface_status("bridge","ipv4-address")
        if true == bup and _M.check_ip(bip) then
            value = _M.ubus_inface_status("bridge",param)
        end
    elseif lproto and "dhcp" == lproto then
        local apcli0mac = util.exec([[iwinfo apcli0 i 2>/dev/null | grep -E "Access Point" | awk '{print $3}' | tr -d '\n']])
        if "" ~= apcli0mac and "00:00:00:00:00:00" ~= apcli0mac then
            value = util.exec([[ifconfig apcli0 2>/dev/null | grep HWaddr | awk '{print $5}' | tr -d '\n']])
        end
    end 
    return string.gsub(value,":","")
end

function _M.get_sn(mac)
    local sn = util.exec([[test -f /usr/bin/sncfg && sncfg -r | tr -d "\n"]])
    if sn and "" ~= sn then
        return sn
    else
        mac = string.gsub(mac,":","")
        local mh = string.sub(mac,1,6)
        local mt = string.sub(mac,7,12)
        return string.format("%08d%08d",tonumber(mh,16),tonumber(mt,16))
    end
end

function _M.get_ip()
    return _M.get_up_network_info("ipv4-address")
end

function _M.get_broadcast()
    return _M.get_up_network_info("broadcast")
end

function _M.mac_pro(p_mac)
    if not p_mac or 12 ~= string.len(p_mac) then
        return p_mac
    end
    local i = 1
    local mac=""
    while 12 > i do
        mac = mac..":"..string.sub(p_mac,i,i+1)
        i = i + 2
    end
    return string.upper(string.sub(mac,2,18))
end

function _M.mac_match(mac)
   if not mac then
      return nil
   end
   local l_mac = string.match(mac,"^%x%x:%x%x:%x%x:%x%x:%x%x:%x%x$")
   if l_mac then
       return l_mac
   end
   l_mac = string.match(mac,"^%x%x%x%x%x%x%x%x%x%x%x%x$")
   return l_mac
end


function _M.http_request_post(p_url,p_arg)
    http.TIMEOUT = 5
    local response_body = {}  
    local post_data = "" 
    local headers_t = { ["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8"}
    if "table" == type(p_arg) then
        post_data = json.encode(p_arg)
    else
        post_data = p_arg or "" 
    end
    headers_t["Content-Length"] = string.len(post_data)
    local res, code = http.request{  
        method = "POST",  
        url = p_url,
        headers = headers_t,
        source = ltn12.source.string(post_data),  
        sink = ltn12.sink.table(response_body)  
    }  
    local result = nil
    if 200 == code then
        result = table.concat(response_body) --must
    end
    return code, result
end

--modify by pixiaocong in 20160308
function _M.https_request_post(p_url,p_arg) --if 'p_arg' is nil, method is 'get'
    local https = require("ssl.pifiihttps")
    local request_body = nil
    if "table" == type(p_arg) then
        request_body = json.encode(p_arg)
    else
        request_body = p_arg 
    end
    local code,res, headers,status = https.request(p_url,request_body)
    return code, res 
end

--end 

function _M.http_request_head(p_url)
    http.TIMEOUT = 10  
    local res, code, head = http.request{  
        method = "HEAD",  
        url = p_url
    }  
    local result = nil
    if 200 == code then
        result = head
    end
    return code, result
end

function _M.get_urlfile_size(p_url) --http
    local size = nil
    local code = nil
    if p_url and "" ~= p_url then
        local c, h = _M.http_request_head(p_url)        
        if h and "table" == type(h) and h["content-length"] then
            size = h["content-length"]
        end
        code = c
    end
    return code,size
end
function _M.get_filesize_ssl(p_url) --https
     if p_url then
        local curl_cmd = "curl -k --head --connect-timeout 5 "..p_url.." 2>&1 | grep -E \"Content-Length:|HTTP/1.1\""
        local reqs_str = _M.exec(curl_cmd)                                                                         
        reqs_str = string.gsub(reqs_str,"%s","")
        local num = nil                         
        local is_ok = string.find(reqs_str,"HTTP/1.1200OK")
        if is_ok then                                      
            local len_str = string.gsub(reqs_str,"HTTP/1.1200OK","")
            local l_num = string.gsub(string.gsub(len_str,'-',""),"ContentLength:","")
            num = string.gsub(l_num," ","")                                           
            if num and "" ~= num and string.find(num,"^%d*$") then
                return 200,num                                    
            end               
        end --if is_ok then
    end --if url then      
    return 0,0 
end

function _M.check_md5code(p_file,p_md5code)
    local result = false
    local md5code_tmp = _M.exec("echo -n $(md5sum "..p_file.." | sed 's/\ .*//g')")
    if p_md5code == md5code_tmp then
        result = true
    end
    return result
end

function _M.get_file_name(str)
  if not str then
    return ""
  else
    return string.match(str,".+/(.+)")
  end
end

function _M.download(p_url,p_dir,p_md5code)
    --[[
       return 
       0:Downloaded success 
       1:Unkown error
       2:Resources not found
       3:Not downloading address
       4:File size error
       5:Md5code check failed
       6:Downloads was interrupted
       7:Local storage directory does not exist
       8:wget or curl is not running
    --]]
    local result = 0 
    if not p_url or "" == string.gsub(p_url," ","") or not p_dir or "" == string.gsub(p_dir," ","") then
        return 1,"Unkown error"
    end
    if 0 ~= os.execute("test -d "..p_dir) then
        return 7,"Local storage directory does not exist"
    end
    local url_all = p_url --url_all="http://pf.pifii.cn/upload/563c7d7fc039d.bin"
    local download_dir = p_dir
    local md5code = nil
    if p_md5code and "" ~= string.gsub(p_md5code," ","") then
        md5code = p_md5code
    end
    local isdownloading = true
    if not _M.get_file_name(url_all) then
        return 3,"Not downloading address" 
    end
    local filename = download_dir.."/".._M.get_file_name(url_all)
    
    repeat           
        local code = nil 
        local file_size = nil
        local is_ssl = string.find(url_all,"^https:")
        if is_ssl then
            code, file_size = _M.get_filesize_ssl(url_all)
        else
            code, file_size = _M.get_urlfile_size(url_all)
        end
        if not code or 200 ~= code then
            return 2,"Resources not found"
        end
        if not file_size then 
            return 3,"Not downloading address" 
        end
        if 0 >= file_size + 0 then
            return 4,"File size error" 
        end
        if 0 == os.execute("test -f "..filename) then 
            if md5code then 
                if _M.check_md5code(filename,md5code) then
                    return 0,filename
                else
                    os.execute("rm -f "..filename)
                end
            else 
                local l_size = _M.exec("echo -n $(ls -l "..filename.." | awk \'{print $5}\')")
                if l_size + 0 == file_size + 0 then
                    return 0,filename
                else
                    os.execute("rm -f "..filename)
                end
            end
        end
        --curl
        if is_ssl then
            os.execute("curl -k -o "..filename.." "..url_all.." >/dev/null 2>&1 &") 
        else
            os.execute("wget -c "..url_all.." -P "..download_dir..">/dev/nul 2>&1 &")
        end 
        local pro_time = 0
        local cur_time = 0 
        local cur_size = 0
        local pro_size = 0
        local pro_time_k = 0
        local cur_time_k = 0
        
        pro_time = os.time()
        while true do
            cur_time = os.time()
            if 0 == os.execute("test -f "..filename) then
                break
            else 
                if (pro_time + 15 < cur_time + 0) then
                    os.execute("kill -15 $(pgrep wget) >/dev/nul 2>&1")
                    os.execute("killall curl >/dev/nul 2>&1")
                    isdownloading = false
                    break
                end
            end
        end
        if not isdownloading then
            return 8,"wget or curl is not running"
        end
        pro_time_k = os.time() 
        while true do
            cur_size = _M.exec("echo -n $(ls -l "..filename.." | awk \'{print $5}\')")
            cur_time_k = os.time() --_M.exec("echo -n $(date +%s)")
            if file_size + 0 == cur_size + 0 then
                if md5code then
                    if _M.check_md5code(filename,md5code) then
                        return 0,filename
                    else 
                        os.execute("test -f "..filename.." && rm -f "..filename)
                        return 5,"Md5code check failed"
                    end
                else
                    return 0,filename
                end
            else
                if pro_size + 0 == cur_size + 0 and pro_time_k + 20 < cur_time_k + 0 then
                    os.execute("killall wget curl >/dev/nul 2>&1") 
                    return 6,"Downloads was interrupted" --wget is stop
                elseif pro_size + 0 < cur_size + 0 then 
                    pro_size = cur_size
                    pro_time_k = cur_time_k 
                end
            end --if file_size + 0 == cur_size + 0
        end--while true
    until true
    return 1
end

function _M.get_software_version()
    return _M.exec("echo -n $(cat /etc/openwrt_version)")
end

function _M.txpower_to_dB(arg)
    local num = tonumber(arg)
    if num then
        num = num / 100 
        return math.ceil(num * 30)
    end
    return 30
end

function _M.txpower_to_percent(arg)
    local num = tonumber(arg)
    if 26 <= num then
        return 100
    else
        num = num / 30
        return math.floor(num * 100)
    end
    return 100
end

return _M
