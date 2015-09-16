-- Rui Santos
-- Complete project details at http://randomnerdtutorials.com

ledOn = 0
pin=4
gpio.mode(pin,gpio.OUTPUT)
uart.on("data", 3, 
    function(data)
        print("Received from Arduino:", data)
        if data=="HI!" then
            if ledOn==0 then
                ledOn = 1
                gpio.write(pin,gpio.HIGH)
            else
                ledOn = 0
                gpio.write(pin,gpio.LOW)
        end        
    end        
end, 0)
