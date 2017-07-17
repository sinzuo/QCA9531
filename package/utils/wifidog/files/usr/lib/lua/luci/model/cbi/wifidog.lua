--[[
	http://www.redwave.cc
]]--

local sys = require "luci.sys"
local fs = require "nixio.fs"
local uci = require "luci.model.uci".cursor()
local wan_ifname = luci.util.exec("uci get network.wan.ifname")
local lan_ifname = luci.util.exec("uci get network.lan.ifname")
m = Map("wifidog", "web认证客户端配置",
	translate("Smart Cloud AP"))
m.on_after_commit = function() luci.sys.call("/etc/init.d/wifidog restart") end  

if fs.access("/usr/bin/wifidog") then
	s = m:section(TypedSection, "wifidog", "web认证配置")
	s.anonymous = true
	s.addremove = false

	wifidog_enable = s:option(Flag, "wifidog_enable", translate("启用认证"),"打开或关闭认证")
	wifidog_enable.rmempty=false
	--wifi_enable.default = wifi_enable.enable
	--wifi_enable.optional = true
	deamo_enable = s:option(Flag, "enable", translate("守护进程"),"开启监护认证进程，保证认证进程时时在线")
	deamo_enable:depends("wifidog_enable","1")	
	ssl_enable = s:option(Flag, "ssl_enable", translate("加密传输"),"启用安全套接层协议传输，提高网络传输安全")
	--[[Peers]]--
	gatewayID = s:option(Value,"gateway_id","门户ID","此处设置认证服务器端配置好的节点id（gateway_id）")
	gateway_interface = s:option(Value,"gateway_interface","内网接口","设置内网接口，默认'br-lan'，或者：eth0, eth1, wlan0, ra0等")
	gateway_interface.default = "br-lan"
	gateway_interface:value(wan_ifname,wan_ifname .."" )
	gateway_interface:value(lan_ifname,lan_ifname .. "")
	
	gateway_eninterface = s:option(Value,"gateway_eninterface","外网接口","此处设置认证服务器的外网接口-默认eth2")
	gateway_eninterface.default = wan_ifname
	gateway_eninterface:value(wan_ifname,wan_ifname .."")
	gateway_eninterface:value(lan_ifname,lan_ifname .. "")

for _, e in ipairs(sys.net.devices()) do
	if e ~= "lo" then gateway_interface:value(e) end
	if e ~= "lo" then gateway_eninterface:value(e) end
end
	
	gateway_hostname = s:option(Value,"gateway_hostname","认证服务器地址","域名或者IP地址")
	gateway_httpport = s:option(Value,"gateway_httpport","认证服务器端口","默认80端口")
	gateway_path = s:option(Value,"gateway_path","认证服务器路径","最后要加/，例如：'/'，'/wifidog/'")
	gateway_connmax = s:option(Value,"gateway_connmax","最大用户接入数量","以路由器性能而定，默认32")
	gateway_connmax.default = "32"
	check_interval = s:option(Value,"check_interval","检查间隔","接入客户端在线检测间隔，默认60秒")
	check_interval.default = "60"
	client_timeout = s:option(Value,"client_timeout","客户端超时","接入客户端认证超时，默认5分")
	client_timeout.default = "5"
	whitelist = s:option(DynamicList,"whitelist","域名白名单")
	--[[Peer]]--
	--s = m:section(TypedSection, "wifidog_deamo", "wifiant监测")
	--s.anonymous = false
	--s.addremove = false
	--xg.default = true
	--xg = s:option(Flag, "xg", translate("是否立即生效"),"选中后，点保存应用，所修改的内容将立即生效")

else
	m.pageaction = false
end

local apply=luci.http.formvalue("cbi.apply")
if apply then
        io.popen("/sbin/2060reset")
end


return m


