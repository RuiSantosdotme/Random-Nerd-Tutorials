-- Rui Santos
-- Complete project details at http://randomnerdtutorials.com

wifi.setmode(wifi.STATION)
wifi.sta.config("YOUR_NETWORK_NAME","YOUR_NETWORK_PASSWORD")
print(wifi.sta.getip())
led1 = 3
led2 = 4
gpio.mode(led1, gpio.OUTPUT)
gpio.mode(led2, gpio.OUTPUT)
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive", function(client,request)
        local buf = "";
        local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
        if(method == nil)then
            _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
        end
        local _, _, auth = string.find(request, "%cAuthorization: Basic ([%w=\+\/]+)");--Authorization:
          if (auth == nil or auth ~= "dXNlcjpwYXNz")then --user:pass
               client:send("HTTP/1.0 401 Authorization Required\r\nWWW-Authenticate: Basic realm=\"ESP8266 Web Server\"\r\n\r\n<h1>Unauthorized Access</h1>");
               client:close();
               return;
          end
        local _GET = {}
        if (vars ~= nil)then
            for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
                _GET[k] = v
            end
        end
        buf = buf.."<head>";
        buf = buf.."<meta charset=\"utf-8\">";
        buf = buf.."<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">";
        buf = buf.."<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
        buf = buf.."<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
        buf = buf.."<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\">";
        buf = buf.."</head><div class=\"container\">";

        buf = buf.."<h1>Web Server</h1>";
        buf = buf.."<h2>GPIO 0</h2>";
        buf = buf.."<div class=\"row\">";
        buf = buf.."<div class=\"col-md-2\"><a href=\"?pin=ON1\" class=\"btn btn-block btn-lg btn-success\" role=\"button\">ON</a></div>";
        buf = buf.."<div class=\"col-md-2\"><a href=\"?pin=OFF1\" class=\"btn btn-block btn-lg btn-danger\" role=\"button\">OFF</a></div>";
        buf = buf.."</div>";
        buf = buf.."<h2>GPIO 2</h2>";
        buf = buf.."<div class=\"row\">";
        buf = buf.."<div class=\"col-md-2\"><a href=\"?pin=ON2\" class=\"btn btn-block btn-lg btn-primary\" role=\"button\">ON</a></div>";
        buf = buf.."<div class=\"col-md-2\"><a href=\"?pin=OFF2\" class=\"btn btn-block btn-lg btn-warning\" role=\"button\">OFF</a></div>";
        buf = buf.."</div></div>";
        
        local _on,_off = "",""
        if(_GET.pin == "ON1")then
              gpio.write(led1, gpio.HIGH);
        elseif(_GET.pin == "OFF1")then
              gpio.write(led1, gpio.LOW);
        elseif(_GET.pin == "ON2")then
              gpio.write(led2, gpio.HIGH);
        elseif(_GET.pin == "OFF2")then
              gpio.write(led2, gpio.LOW);
        end
        client:send(buf);
        client:close();
        collectgarbage();
    end)
end)
