-- Rui Santos
-- Complete project details at http://randomnerdtutorials.com
-- ESP8266 Server

print("ESP8266 Server")
wifi.setmode(wifi.STATIONAP);
wifi.ap.config({ssid="test",pwd="12345678"});
print("Server IP Address:",wifi.ap.getip())

sv = net.createServer(net.TCP) 
sv:listen(80, function(conn)
    conn:on("receive", function(conn, receivedData) 
        print("Received Data: " .. receivedData)         
    end) 
    conn:on("sent", function(conn) 
      collectgarbage()
    end)
end)
