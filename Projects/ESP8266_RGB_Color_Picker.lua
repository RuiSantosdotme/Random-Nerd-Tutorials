-- Rui Santos
-- Complete project details at http://randomnerdtutorials.com

wifi.setmode(wifi.STATION)
wifi.sta.config("YOUR_NETWORK_NAME","YOUR_NETWORK_PASSWORD")

tmr.delay(5000)
print(wifi.sta.getip())

function led(r, g, b)
    pwm.setduty(16, r)
    pwm.setduty(5, g)
    pwm.setduty(4, b)
end
pwm.setup(16, 1000, 1023)
pwm.setup(5, 1000, 1023)
pwm.setup(4, 1000, 1023)
pwm.start(16)
pwm.start(5)
pwm.start(4)

srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive", function(client,request)
        local buf = "";
        buf = buf.."HTTP/1.1 200 OK\n\n"
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
        buf = buf.."<!DOCTYPE html><html><head>";
        buf = buf.."<meta charset=\"utf-8\">";
        buf = buf.."<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">";
        buf = buf.."<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
        buf = buf.."<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css\">";
        buf = buf.."<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js\"></script>";
        buf = buf.."<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>";
        buf = buf.."</head><body><div class=\"container\"><div class=\"row\"><h1>ESP Color Picker</h1>";       
        buf = buf.."<a type=\"submit\" id=\"change_color\" type=\"button\" class=\"btn btn-primary\">Change Color</a> ";
        buf = buf.."<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\"></div></div>";
        buf = buf.."<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);";      
        buf = buf.."document.getElementById(\"change_color\").href=\"?r=\" + Math.round(picker.rgb[0]*4.0117) + \"&g=\" +  Math.round(picker.rgb[1]*4.0117) + \"&b=\" + Math.round(picker.rgb[2]*4.0117);}</script></body></html>";
        
        if(_GET.r or _GET.g or _GET.b) then
            -- This is for RGB Common Cathode
            -- led(_GET.r, _GET.g,_GET.b)
            
            -- This is for RGB Common Anode
            led(1023-_GET.r, 1023-_GET.g,1023-_GET.b)   
        end
        client:send(buf);
        client:close();
        collectgarbage();
    end)
end)
