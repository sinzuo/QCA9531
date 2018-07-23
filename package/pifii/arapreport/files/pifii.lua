--PiFii library
--local FM_TYPE = "PiFii"

local sys = require("luci.sys")
local http = require("socket.http") 
local ltn12 = require("ltn12")
local json = require("luci.json")
local util = require ("luci.util")
require "luci.model.uci"
local uci = luci.model.uci.cursor()
local _M = {}

--_M.fm_type = FM_TYPE

_M.debug = 0
--[[
local sysinfo = nil
if "PiFii" == _M.fm_type then
    sysinfo = util.ubus("system","info") or { } 
end
--]]

function _M.flag_register()
    return true 
end

function _M.http_header()
    local http_headers_t = {
        ["User-Agent"] = "wifi",
        ["Content-Type"] = "application/json" 
    }
    return http_headers_t
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

function _M.get_cmd_output(cmd,drop_sum,fs)                                                                                       
    local fd = io.popen(cmd, "r");                                                   
    local res = {}   
    local row=nil
    if fd then 
        for i=1,drop_sum,1 do
            fd:read("*l")  
        end
          
        while true do                                                                
            local line = fd:read("*l")                                               
            if not line or #line==0 then break end                                   
            row=_M.split(line,fs)
			table.insert(res,row)
        end                                                                          
    end
    fd:close()	
    return res                                                   
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


function _M.equals_table(t_one,t_two)
    local result=false
    if not t_one then
        t_one = {}
    end
    if not t_two then
        t_two = {}
    end
    
    table.sort(t_one)
    table.sort(t_two)
    local one_str=json.encode(t_one)
    local two_str=json.encode(t_two)
    if one_str and two_str and one_str == two_str then
        result=true
    end
    return result
end       
--[[
function _M.postHttp(url_str,post_arg)
    local http = require("socket.http")
    local ltn12 = require("ltn12")
    http.TIMEOUT = 5
    local response_body = {}
    local post_data = post_arg or ""
    local res, code = http.request{
        method = "POST",
        url = url_str,
        headers =
        {
           ["User-Agent"] = "cloudfi 1.0",
           ["Content-Type"] = "application/json;charset=UTF-8",
           ["Content-Length"] = string.len(post_data)
        },
        source = ltn12.source.string(post_data),
        sink = ltn12.sink.table(response_body)
    }
    local result
    if 200 == code then
        result = table.concat(response_body)
    end
    return code, result
end
--]]
function _M.http_request_post(p_url,p_headers,p_arg)
    http.TIMEOUT = 5
    local response_body = {}  
    local post_data = "" 
    local headers_t = {}
    if "table" == type(p_headers) then
        headers_t = p_headers
    else
        headers_t["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8"
    end
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
        local reqs_str = util.exec(curl_cmd)                                                                         
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
    local md5code_tmp = util.exec("echo -n $(md5sum "..p_file.." | sed 's/\ .*//g')")
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

function _M.wget_download(p_url,p_dir,p_md5code)
    --[[
        0:下载成功,
        1:其它错误,
        2:下载地址错误或网络不通或资源没找到,
        3:非下载地址,
        4:文件大小异常,
        5:需要md5校验但校验失败,
        6:下载中断,
        7:指定的下载目录不存在
    --]]
    local result = 0 
    if not p_url or "" == string.gsub(p_url," ","") or not p_dir or "" == string.gsub(p_dir," ","") then
        return 1
    end
    if 0 ~= os.execute("test -d "..p_dir) then
        return 7
    end
    local url_all = p_url --url_all="http://www.cloudfi.cn/upload/firmware/563c7d7fc039d.bin" --url = p_url
    local download_dir = p_dir
    local md5code = nil
    if p_md5code and "" ~= string.gsub(p_md5code," ","") then
        md5code = p_md5code
    end
    local isdownloading = true
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
            return 2 --2:下载地址错误或网络不通或资源没找到
        end
        if not file_size then 
            return 3 --3:非下载地址
        end
        if 0 >= file_size + 0 then
            return 4 --4:文件大小异常
        end
        if 0 == os.execute("test -f "..filename) then 
            if md5code then 
                if _M.check_md5code(filename,md5code) then
                    return 0,filename
                else
                    os.execute("rm -f "..filename)
                end
            else 
                local l_size = util.exec("echo -n $(ls -l "..filename.." | awk \'{print $5}\')")
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
        else  --使用wget命令下载
            os.execute("wget -c "..url_all.." -P "..download_dir..">/dev/nul 2>&1 &")
        end 
        local pro_time = 0 --= util.exec("echo -n $(date +%s)")
        local cur_time = 0 
        local cur_size = 0
        local pro_size = 0
        local pro_time_k = 0
        local cur_time_k = 0
        
        --确定文件已经开始下载
        pro_time = os.time()--util.exec("echo -n $(date +%s)")
        while true do
            cur_time = os.time()--util.exec("echo -n $(date +%s)") + 0
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
            return 1 --wget is not running
        end
        while true do
            cur_size = util.exec("echo -n $(ls -l "..filename.." | awk \'{print $5}\')")
            cur_time = util.exec("echo -n $(date +%s)")
            cur_time_k = cur_time
            --文件已下载完成
            if file_size + 0 == cur_size + 0 then
                if md5code then
                    if _M.check_md5code(filename,md5code) then
                        return 0,filename
                    else --md5码校验错误
                        os.execute("test -f "..filename.." && rm -f "..filename)
                        return 5 --"error: md5code is wrong
                    end --if check_md5code(filename,md5code)
                else
                    return 0,filename
                end
            else
                if pro_size + 0 == cur_size + 0 and pro_time_k + 20 < cur_time_k + 0 then
                    os.execute("killall wget curl >/dev/nul 2>&1") 
                    return 6 --wget is stop
                elseif pro_size + 0 < cur_size + 0 then 
                    pro_size = cur_size
                    pro_time_k = cur_time_k 
                end
            end --if file_size + 0 == cur_size + 0
        end--while true
    until true
    return 1
end

function _M.get_cpuload()
    local cpuload = 0 
    cpuload = util.exec("top -n 1 | head -n 2 | grep CPU: | awk \'{print(100-$8)}\' | tr -d \'\n\'")
    return cpuload
end

function _M.get_freeram()
    local sysinfo = util.ubus("system","info") or { }
    local meminfo = sysinfo.memory or {                                  
                total = 0,                                                   
                free = 0,                                                    
                buffered = 0,                                                
                shared = 0                                                   
        }  
    return meminfo["free"]
end

function _M.get_sysuptime()
    local sysinfo = util.ubus("system","info") or { }
    return sysinfo.uptime
end

function _M.get_confversion()
    uci:load("pifii")
    return uci:get("pifii", "confinfo", "conf_version") or ""
end

function _M.get_channel()
    uci:load("wireless")
    return uci:get_first("wireless", "wifi-device", "channel", "auto")
end

function _M.get_manufacturer()
    local res = uci:get_first("pifii","device","factory","")
    --if "" == res then
    --    res = util.exec("echo -n $(head -n 1 /tmp/sysinfo/model | awk \'{print $1}\')")
    --end
    return res
end

function _M.get_ap_model()
    local res = util.exec("cat /etc/openwrt_release  | grep DISTRIB_CODENAME | tr -d \"DISTRIB_CODENAME=|\\'|\\n\" | tr -s \'[a-z]\' \'[A-Z]\'")
    local wd = uci:get("wireless","wifi1") or ""
    if "AP100" == res then                     
        if "" ~= wd then res = "AP200" end
    elseif "AC100" == res then
        if "" ~= wd then res = "AC200" end
    elseif "AH100" == res then
        if "" ~= wd then res = "AH200" end
    end
    return res
end

function _M.get_software_version()
    return util.exec("echo -n $(cat /etc/openwrt_version)")
end

function _M.get_ap_ip()
    local proto = uci:get("network","lan","proto") or ""
    local ifa = util.ubus("network.interface.lan","status")
    local res = "" 
    if "dhcp" == ifa.proto then
        if ifa["ipv4-address"] and ifa["ipv4-address"][1] then
            res = ifa["ipv4-address"][1]["address"] or ""
        end
    else
        ifa = util.ubus("network.interface.wan","status")
        if ifa["ipv4-address"] and ifa["ipv4-address"][1] then
            res = ifa["ipv4-address"][1]["address"] or "" 
        end
    end
    return res
end

function _M.get_ap_mac()
    --local cmd="dd bs=1 skip=4 count=6 if=/dev/mtdblock2 2>/dev/null | hexdump -vC | head -n 1 | awk \'{print $2$3$4$5$6$7}\' | tr -d \"\\n\""
    local cmd="ifconfig br-lan | grep HWaddr | awk \'{print $5}\' | tr -d \"\\n\|:\""
    local mac=util.exec(cmd)
    return mac 
end

function _M.clean_log(logfile)
    if logfile then
        local util = require("luci.util")
        if 0 == os.execute("test -f "..logfile) then
            local num = luci.util.exec("wc -l "..logfile.." 2>&1 | awk \'{print $1}\' ")
            num = tonumber(num) or 1
            if 5000 <= num + 0 then
               os.execute("echo -n \' \' > "..logfile)
            end
        end
    end
end

function _M.get_ifname(ifname)
    local w_if = nil
    if ifname then
        local if_l = _M.split(ifname," ")
        local vid = nil
        for _,v in pairs(if_l) do
            vid=string.gsub(v,"eth0.","")
            uci:foreach("network","switch_vlan",function(s)
                 local l_name = s[".name"]
                 local l_vid = uci:get("network",l_name,"vid")
                 local l_ports = uci:get("network",l_name,"ports")
                 if not w_if and vid == l_vid and string.find(l_ports,"4") then
                    w_if = "eth0."..vid
                 end
               end)
        end
    end
    return w_if  
end

return _M
