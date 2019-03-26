local nt=require "luci.sys".net                                                     

local uci = require "luci.model.uci".cursor()
local sectionname=uci:get_first("sets","model")
local proto=uci:get("network","lan","proto")
local model=uci:get_first("sets","model","enable")
if sectionname then
    if "dhcp" == proto and (not model or "1" == model) then
        uci:set("sets",sectionname,"enable",0)
        uci:commit("sets")
    elseif "static" == proto and (not model or "0" == model) then
        uci:set("sets",sectionname,"enable",1)
        uci:commit("sets")
    end
end
                                                                                    
local m,s,o,mp,ml,flag                                                              
                        


m=Map("sets")

m:chain("luci")

s=m:section(TypedSection,"model",translate("模式切换"))

s.anonymous=true


s:tab("model",translate("系统运行模式"))
o=s:taboption("model",ListValue,"enable",translate("模式选择"))
o:value("0",translate("AP模式"))
o:value("1",translate("路由模式"))                                                    
--o.default =0                                                                
--o.datatype="uinteger"                                                       
--o:value(0,translate("off"))                                                 
--o:value(1,translate("on")) 

--ml = s:taboption("loginctl", DynamicList, "maclist", translate("user list")) 
--nt.mac_hints(function(mac, name) ml:value(mac, "%s (%s)" %{ mac, name }) end)  



local apply=luci.http.formvalue("cbi.apply")
if apply then
        io.popen("/sbin/modelchange")
end

return m

