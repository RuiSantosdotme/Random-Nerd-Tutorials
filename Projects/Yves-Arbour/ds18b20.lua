--------------------------------------------------------------------------------
-- DS18B20 one wire module for NODEMCU
-- LICENCE: http://opensource.org/licenses/MIT
-- Vowstar <vowstar@nodemcu.com>
-- Dramatic simplification: Peter Scargill
--------------------------------------------------------------------------------

-- Set module name as parameter of require
local modname = ...
local M = {}
_G[modname] = M
--------------------------------------------------------------------------------
-- Local used modules
--------------------------------------------------------------------------------
-- Table module
local table = table
-- String module
local string = string
-- One wire module
local ow = ow
-- Timer module
local tmr = tmr
-- Limited to local environment
setfenv(1,M)
--------------------------------------------------------------------------------
-- Implementation – you don’t get any shorter than this
--------------------------------------------------------------------------------

function readNumber(pin)
        ow.setup(pin)
        ow.reset(pin)
        ow.write(pin, 0xCC, 1)
        ow.write(pin, 0xBE, 1)
        data = nil
        data = ""
        for i = 1, 2 do
            data = data .. string.char(ow.read(pin))
        end
        t = (data:byte(1) + data:byte(2) * 256) / 16
        if (t>100) then
        t=t-4096
        end
        ow.reset(pin)
        ow.write(pin,0xcc,1)
        ow.write(pin, 0x44,1)  
        return t          
end

-- Return module table
return M
