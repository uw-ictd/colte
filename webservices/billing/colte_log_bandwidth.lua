--
-- (C) 2014-15-15 - ntop.org
--
my_subnet = "192.168.151.0/24"
my_chunks = {my_subnet:match("(%d+)%.(%d+)%.(%d+)%.(%d+)/(%d+)")}

function check_ipv4(ip)

	-- must pass in a string value
	if ip == nil or type(ip) ~= "string" then
		return 0
	end
	-- check for format 1.11.111.111 for ipv4.
	-- first, break into chunks. then make sure there are 4. then make sure each is 0 < n < 255
	local chunks = {ip:match("(%d+)%.(%d+)%.(%d+)%.(%d+)")}
	
	if (#chunks ~= 4) then
		return 0
	end
	
	for _,v in pairs(chunks) do
		if (tonumber(v) < 0 or tonumber(v) > 255) then
			return 0
		end
	end

	for i = 1, 4 do
		local x = tonumber(chunks[i])
		local y = tonumber(my_chunks[i])
		if (y ~= 0 and x ~= y) then
--		print(x .. ' ' .. y)
--		if (x ~= y) then
--			print(ip .. ' NOT IN SUBNET \n')
			return 0
		end
	end
	return 1
end
	
local dirs = ntop.getDirs()
package.path = dirs.installdir .. "/scripts/lua/modules/?.lua;" .. package.path

require "lua_utils"

sendHTTPContentTypeHeader('text/html')

-- local host_ip = _GET["10.42.0.77"]
local host_ip = "192.168.151.2"

interface.select(ifname)
local host = interface.getHostInfo(host_ip)
local value = 0

local t = os.time()
print('time ' .. t .. '\n')

if(host ~= nil) then
   value = host["packets.sent"]+host["packets.rcvd"]
   v1 = host["bytes.sent"]
   v2 = host["bytes.rcvd"]
   print(host_ip .. ' ' .. v1 .. ' ' .. v2 .. '\n')
end


local hosts_stats = interface.getHostsInfo()
hosts_stats = hosts_stats["hosts"]
for k, v in pairs(hosts_stats) do
	if(check_ipv4(k) == 1) then
		print(k .. ' ' .. v["bytes.sent"] .. ' ' .. v["bytes.rcvd"] .. '\n')
--	else
--		print(k .. ' NOT IPV4!!!\n')
	end
end

-- print('TEST\n')
-- print(' { "value": ' .. value .. ' } ')
-- print(' { "v1": ' .. v1 .. ' } ')

-- for k,v in pairs(host) do
-- 	print(' {KEY: ' .. k .. ' } ')
-- end

-- file = io.open("/home/spencer/Desktop/TESTLUA.lua", "a")
-- io.output(file)
-- io.write("HELLO")
-- io.close(file)
