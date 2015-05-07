wifi.setmode(wifi.STATION)
wifi.sta.config("YOUR_NETWORK_NAME","YOUR_NETWORK_PASSWORD")
print(wifi.sta.getip())
tmr.delay(5000)

sensorType="dht11"             -- set sensor type dht11 or dht22
pin = 4 --  data pin, GPIO2
humi=0
temp=0
fare=0
bimb=1
--load DHT module for read sensor
function ReadDHT11()
    dht=require("dht")
    dht.read(pin)
    chck=1
    h=dht.getHumidity()
    t=dht.getTemperature()
    if h==nil then h=0 chck=0 end
    if sensorType=="dht11"then
        humi=h/256
        temp=t/256
    else
        humi=h/10
        temp=t/10
    end
    fare=(temp*9/5+32)
    print("Humidity:    "..humi.."%")
    print("Temperature: "..temp.." deg C")
    print("Temperature: "..fare.." deg F")
    -- release module
    dht=nil
    package.loaded["dht"]=nil
end
ReadDHT11()

tmr.alarm(1,5000, 1, function() ReadDHT11() bimb=bimb+1 if bimb==5 then bimb=0 wifi.sta.connect() print("Reconnect")end end)

srv=net.createServer(net.TCP) srv:listen(80,function(conn)
    conn:on("receive",function(conn,payload)
   --print(payload) -- for debugging only
    --generates HTML web site
    conn:send('HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nCache-Control: private, no-store\r\n\r\n\
        <!DOCTYPE HTML>\
        <html><head><meta name="viewport" content="width=device-width, initial-scale=1"></head>\
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css">\
        <meta http-equiv="refresh" content="6">\
        </head><div class="container">\
        <h1>Sensor Data</h1></br><div class="row">\
        <div class="col-md-4"><div class="panel panel-primary"><div class="panel-heading"><h3 class="panel-title">Humidity</h3>\
        </div><div class="panel-body">\
        <div class="form-group form-group-lg"><input type="text" class="form-control" value="'..humi..' %">\
        </div></div></div></div>\
        <div class="col-md-4"><div class="panel panel-info"><div class="panel-heading"><h3 class="panel-title">Temperature</h3>\
        </div><div class="panel-body">\
        <div class="form-group form-group-lg"><input type="text" class="form-control" value="'..temp..' º C">\
        <input type="text" class="form-control" value="'..fare..' º F">\
        </div></div></div></div></div></div></html>')
    conn:on("sent",function(conn) conn:close() end)
    end)
end)
