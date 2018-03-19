--
-- (C) 2014-15-15 - ntop.org
--

local dirs = ntop.getDirs()
package.path = dirs.installdir .. "/scripts/lua/modules/?.lua;" .. package.path

require "lua_utils"

sendHTTPContentTypeHeader('text/html')

-- local host_ip = _GET["10.42.0.77"]
local host_ip = "10.42.0.77"

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
