-- Rui Santos
-- Complete project details at http://randomnerdtutorials.com

wifi.setmode(wifi.STATION)
wifi.sta.config("YOUR_NETWORK_NAME","YOUR_NETWORK_PASSWORD")

-- A simple http client
conn = nil
conn=net.createConnection(net.TCP, 0)
conn:on("receive", function(conn, payload) end)
conn:connect(80,"maker.ifttt.com")
conn:on("connection", function(conn, payload)
conn:send("POST /trigger/motion_detected/with/key/YOUR_API_KEY HTTP/1.1\r\nHost: maker.ifttt.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n") end)
conn:close()
print('Posted Tweet')
