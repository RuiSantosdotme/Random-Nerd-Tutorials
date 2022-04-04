
"""
OTA updater for ESP32 running Micropython by David Flory.
Tutorial: https://randomnerdtutorials.com/esp32-esp8266-micropython-ota-updates/

This is a simple way to run a python program on the ESP32 and download updates or a different program
to run from an HTTP server, either from the web or your local network.

Put all the imports needed for internet in boot, plus a global variable called OTA
The main file creates the internet connection then runs whatever program is required. I call the program 'program.py'

program.py is the main program, and can be whatever you want. The only requisite for this to work with OTA is that
it must be imported into the module 'main' and it must check periodically for updates.
The Program should have a function that can be called by 'main' to start it.

We can overwrite program.py while it is running, because it has been imported on boot into the namespace of 'main'.
All that is required is a reboot after a new Program.py is downloaded. In this case the deep sleep function will reboot the ESP32.

"""
#When prog.py finishes, if OTA = True, main downloads it and overwrites program.py, then effectively reboots.
#If OTA is false main just exits.

#this is the program to be executed. Note we do not use the '.py' extension.
import program

import machine #needed for the deep sleep function

#URLs for the updates
#replace with your PHP server URL or local IP address (Raspberry Pi IP Address)
upd_url="http://192.168.1.XXX/get_ESP_data.php?file=program.py"


#change your wifi credentials here. 
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

print('OTA is ' + str(OTA))#debug

#here we set up the network connection
station = network.WLAN(network.STA_IF)
station.active(False)
station.active(True)
#station.config(reconnects = 5)

station.connect(ssid,password)

while station.isconnected() == False:
  pass

#print board local IP address
print(station.ifconfig())

if OTA == 0:
    print('starting program')
    OTA = program.mainprog(OTA)
    
#mainprog() is the starting function for my program. OTA is set to 0 on boot so the first time this code
#is run, it sets up the network connection and then runs the program.
#The following code only runs when program.py exits with OTA = 1

if OTA == 1:
    print('Downloading update')
    #download the update and overwrite program.py
    response = requests.get(upd_url)
    x = response.text.find("FAIL")
    if x > 15:
        #download twice and compare for security
        x = response.text
        response = requests.get(upd_url)
        if response.text == x:
            f = open("program.py","w")
            f.write(response.text)
            f.flush()
            f.close
            
            #soft reboot 
            print('reboot now')
            machine.deepsleep(5000)
