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
conn:send("POST /trigger/post_tweet/with/key/YOUR_API_KEY HTTP/1.1\r\n"..
  "Host: maker.ifttt.com\r\nConnection: close\r\nAccept: */*\r\nContent-Type: application/json\r\n" ..
  "Content-Length: 17\r\n\r\n{\"value1\":true}\r\n") end)
conn:close()
print('Posted Tweet')
