-- Rui Santos
-- Complete project details at http://randomnerdtutorials.com

wifi.setmode(wifi.STATION)
wifi.sta.config("YOUR_NETWORK NAME","YOUR_NETWORK_PASSWORD")
print(wifi.sta.getip())
tmr.delay(5000)

led = 3
gpio.mode(led, gpio.OUTPUT)

sensorType="dht11"             -- set sensor type dht11 or dht22
pin = 4 --  data pin, GPIO2
humi=0
temp=0
fare=0

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
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive", function(client,request)
        local buf = "";
        local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
        if(method == nil)then
            _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
        end
        local _GET = {}
        if (vars ~= nil)then
            for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
                _GET[k] = v
            end
        end
        buf = buf.."<head>";
        buf = buf.."<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
        buf = buf.."<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
        buf = buf.."<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
        buf = buf.."</head><div class=\"container\">";
        buf = buf.."<div class=\"row\">";
        buf = buf.."<div class=\"col-md-2\"><a href=\"?pin=ON\" class=\"btn btn-block btn-lg btn-success\" role=\"button\">ON</a></div>";
        buf = buf.."<div class=\"col-md-2\"><a href=\"?pin=OFF\" class=\"btn btn-block btn-lg btn-danger\" role=\"button\">OFF</a></div>";
        buf = buf.."<div class=\"col-md-4\"><div class=\"panel panel-primary\"><div class=\"panel-heading\"><h3 class=\"panel-title\">Sensor Data</h3>";
        buf = buf.."</div><div class=\"panel-body\"><div class=\"form-group form-group-lg\"><input type=\"text\" class=\"form-control\" value=\""..humi.."\">";
        buf = buf.."<input type=\"text\" class=\"form-control\" value=\""..temp.." º C\">";
        buf = buf.."<input type=\"text\" class=\"form-control\" value=\""..fare.." º F\">";
        buf = buf.."<a href=\"?pin=UP\" class=\"btn btn-block btn-lg btn-primary\" role=\"button\">Update</a>";
        buf = buf.."</div></div></div></div>";
        buf = buf.."</div></div>";
        
        local _on,_off = "",""
        if(_GET.pin == "ON")then
              gpio.write(led, gpio.HIGH);
        elseif(_GET.pin == "OFF")then
              gpio.write(led, gpio.LOW);
        elseif(_GET.pin == "UP")then
              ReadDHT11();
        end
        client:send(buf);
        client:close();
        collectgarbage();
    end)
end)
