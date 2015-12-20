-- Rui Santos - ESP8266 Server
-- Modified by Yves Arbour to ennable print string to go directly to an Excel sheet with Things Gateway

print("ESP8266 Server")
wifi.setmode(wifi.STATIONAP);
wifi.ap.config({ssid="test",pwd="12345678"});
print("Server IP Address:",wifi.ap.getip())
 
sv = net.createServer(net.TCP)
sv:listen(80, function(conn)
    conn:on("receive", function(conn, receivedData)
      --print("Received Data"..receivedData)
        print(receivedData)-- string "Received Data" removed...
        --Things Gateway ignores strings that do not start with proper command...
        --XLS in this case for Excel sheet        
    end)
    conn:on("sent", function(conn)
      collectgarbage()
    end)
end)
