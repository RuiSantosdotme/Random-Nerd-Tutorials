wifi.setmode(wifi.STATION)
wifi.sta.config("YOUR_NETWORK_NAME","YOUR_NETWORK_PASSWORD")
print(wifi.sta.getip())
tmr.delay(5000)

pin = 4
bimb = 1
fare = 0
fare_dec = 0

--Read DHT Sensor
function ReadDHT11()
    status, temp, humi, temp_dec, humi_dec = dht.read(pin)
    if status == dht.OK then
        print(string.format("DHT Temperature:%d.%03d;Humidity:%d.%03d\r\n",
              math.floor(temp),
              temp_dec,
              math.floor(humi),
              humi_dec
        ))
        fare = (9 * math.floor(temp) / 5) + 32
        fare_dec = (9 * temp / 5) % 10
    elseif status == dht.ERROR_CHECKSUM then
        print( "DHT Checksum error." )
    elseif status == dht.ERROR_TIMEOUT then
        print( "DHT timed out." )
    end
end
ReadDHT11()

tmr.alarm(1,5000, 1, function() ReadDHT11() bimb=bimb+1 if bimb==5 then bimb=0 wifi.sta.connect() print("Reconnect")end end)

srv=net.createServer(net.TCP) srv:listen(80,function(conn)
    conn:on("receive",function(conn,payload)
   --print(payload) -- for debugging only
    --generates HTML web site
    conn:send('HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nCache-Control: private, no-store\r\n\r\n\
        <!DOCTYPE HTML>\
        <html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"></head>\
        <meta http-equiv="X-UA-Compatible" content="IE=edge">\
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">\
        <meta http-equiv="refresh" content="6">\
        </head><div class="container">\
        <h1>Sensor Data</h1></br><div class="row">\
        <div class="col-md-4"><div class="panel panel-primary"><div class="panel-heading"><h3 class="panel-title">Humidity</h3>\
        </div><div class="panel-body">\
        <div class="form-group form-group-lg"><input type="text" class="form-control" value="'..math.floor(humi)..'.'..humi_dec..' %">\
        </div></div></div></div>\
        <div class="col-md-4"><div class="panel panel-info"><div class="panel-heading"><h3 class="panel-title">Temperature</h3>\
        </div><div class="panel-body">\
        <div class="form-group form-group-lg"><input type="text" class="form-control" value="'..math.floor(temp)..'.'..temp_dec..' deg C">\
        <input type="text" class="form-control" value="'..fare..'.'..fare_dec..' deg F">\
        </div></div></div></div></div></div></html>')
    conn:on("sent",function(conn) conn:close() end)
    end)
end)
