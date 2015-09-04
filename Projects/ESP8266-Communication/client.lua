-- Rui Santos
-- Complete project details at http://randomnerdtutorials.com
-- ESP8266 Client

print("ESP8266 Client")
wifi.sta.disconnect()
wifi.setmode(wifi.STATION) 
wifi.sta.config("test","12345678") -- connecting to server
wifi.sta.connect() 
print("Looking for a connection")

tmr.alarm(1, 2000, 1, function()
     if(wifi.sta.getip()~=nil) then
          tmr.stop(1)
          print("Connected!")
          print("Client IP Address:",wifi.sta.getip())
          cl=net.createConnection(net.TCP, 0)
          cl:connect(80,"192.168.4.1")
          tmr.alarm(2, 5000, 1, function() 
            cl:send("Hello World!") 
          end)
      else
         print("Connecting...")
      end
end)
