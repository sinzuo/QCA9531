module("luci.controller.Sets",package.seeall)


function index()
	
	local page


	page=entry({"admin","network","Sets"},
		alias("admin","network","Sets","model"),
		_("设置"),60)

        page=entry({"admin","network","Sets","model"}, 
                arcombine(cbi("set/model"),70),        
                _("设备模式")) 
end


