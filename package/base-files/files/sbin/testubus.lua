require "ubus"
local conn=ubus.connect()
if  not  conn then
--	os.execute("echo 'ayuq'>/tmp/test1")

	os.execute("reboot")
	--os.execute("echo 'ayu'>/root/test")
	--error("faild to connect to ubusd")
end

--os.execute("echo 'ayu'>/root/test")

conn:close()

