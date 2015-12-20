-- Yves Arbour wireless weather station project with data logging to Excel sheets with Things Gateway
-- code based on the following
-- Rui Santos - ESP8266 Client
-- DS18B20 one wire module for NODEMCU 
-- Vowstar <vowstar@nodemcu.com> 
-- Peter Scargill 
-- Wimp Weather Station by sparkfun Nathan Seidle

wifi.sta.disconnect()
wifi.setmode(wifi.STATION)
wifi.sta.config("test","12345678")
wifi.sta.connect() 
print("Looking for a connection")
temp=0
function readds18b20sensor()
    t=require("ds18b20")   
   -- print(t.readNumber(2))  for debuging
    temp = (t.readNumber(2)) --GPIO 04
end

winDirection=0
gpio.mode(0,gpio.INPUT)
function readWindDirection ()
    windDirection = adc.read(0)
        if (windDirection < 26) then windDirection=90 --E
       --if (windDirection < 17) then windDirection=113      -- ESE
       --elseif (windDirection < 21) then windDirection=68   -- ENE
       --elseif (windDirection < 26) then windDirection=90   -- E
       --elseif (windDirection < 36) then windDirection=158  -- SSE
       elseif (windDirection < 43) then windDirection=135 --SE
       --elseif (windDirection < 60) then windDirection=203  -- SSW
       elseif (windDirection < 85) then windDirection=180 --S
       --elseif (windDirection < 98) then windDirection=23   -- NNE
       elseif (windDirection < 120) then windDirection=45 --NE
       --elseif (windDirection < 170) then windDirection=248  -- WSW
       elseif (windDirection < 210) then windDirection=225 -SW
       --elseif (windDirection < 275) then windDirection=338  -- NNW
       elseif (windDirection < 380) then windDirection=0 --N
       --elseif (windDirection < 500) then windDirection=293  -- WNW
       elseif (windDirection < 750) then windDirection=315 --NW
       elseif (windDirection < 1005) then windDirection=270 --W
       else windDirection=-1
       end
end
irqPin = 1  --GPIO5
windSpeed = 0
windClicks = 0
lastWindCheck = 0
function debounce (func)   
    local delay = 100000   
    return function (...)  
        now = tmr.now() 
        now = now - lastWindCheck               
        if now < delay then return end
        lastWindCheck = tmr.now()        
        return func (...)
    end
end

function windSpeedIrq ()    
    windClicks = windClicks+1 * 10000000
    print (windClicks)
end  
function calcWindSpeed ()
    local deltaTime = tmr.now () - lastWindCheck        
    windSpeed = windClicks / deltaTime
    print(deltaTime)              
    windSpeed = windSpeed * 1492 /10000 -- Replace " *1492/10000 " with " *24/100 " for Km/h instead of Mp/h
    windClicks = 0
    lastWindCheck = tmr.now()
end 
gpio.mode(irqPin,gpio.INT)
gpio.trig(irqPin,"down",debounce(windSpeedIrq))
      
file.open("counterMax.lua","r")
counter=(file.read())
file.close()
  tmr.alarm(0, 2000, 1, function()
   if(wifi.sta.getip()~=nil) then
          tmr.stop(0)
          print("Client IP Address:",wifi.sta.getip())
          cl=net.createConnection(net.TCP, 0)
          cl:connect(80,"192.168.4.1")
          tmr.alarm(1, 5000, 1, function()
          counter=counter+1       
       
        readds18b20sensor()            
        readWindDirection()
        calcWindSpeed()
        print (temp)
        print(windSpeed)
-- Next is the properly formated string that Things Gateway needs to show Date,Time,Wind speed,Wind direction and  Temp.    
  cl:send("XLS,write,Example,A"..counter..",%date%\nXLS,write,Example,B"..counter..",%time%\nXLS,write,Example,C"..counter..","..windSpeed.."\nXLS,write,Example,D"..counter..","..windDirection.."\nXLS,write,Example,E"..counter..","..temp)
  
  cl:on("disconnection",function(reconnect)
    file.open("counterMax.lua","w+")
    file.writeline(counter)
    file.close()
   end)
       end)
   else
         print("Connecting...")
       end
end)
function reconnect()    
wifi.setmode(wifi.STATION)
wifi.sta.config("test","12345678")
wifi.sta.connect()
end  
