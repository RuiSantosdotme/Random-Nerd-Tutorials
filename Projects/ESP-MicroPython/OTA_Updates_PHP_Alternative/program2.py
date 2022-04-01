# OTA updater for ESP32 running Micropython by David Flory
# Tutorial: https://randomnerdtutorials.com/esp32-esp8266-micropython-ota-updates

#This program essentially identical to program1, except the tasks are slightly
#different to demonstrate a different program is now running.

import urequests
from utime import sleep
#The URL to use to get updates, full path to the PHP script. The update file must be in the same
#directory as the PHP script.
####Updated name of update files to program1.py, program2.py, program3.py etc. The number denotes something
####like a version number
upd_url="http://192.168.1.XXX/get_ESP_data.php?file=program3.py"
### no longer used as we will not delete the files
#del_url = "http://192.168.1.XXX/delete_ESP_data.php?file=program.py"

def check_for_updates(OTA):
    try:
        #print ('Checking for updates')
        response = urequests.get(upd_url)
        x = response.text.find("FAIL")
        if x > 15:
            OTA = 1
            print('There is an update available')
            return(OTA)
        else:
            print('There are no updates available.')
            return(OTA)

    except:
        print('unable to reach internet')
        return(OTA)


def mainprog(OTA):
    print('Mainprog - OTA is' + str(OTA))
    #This is the entry point for your program code
    ## we no longer delete the files so comment out the next line
    #response = urequests.get(del_url)
    print('Program start')
    while OTA == 0:
        program_tasks()
        OTA = check_for_updates(OTA)
        if OTA == 1:
           return(OTA)
        print('OTA = ' + str(OTA))

def program_tasks():
    #do program tasks. If continuous loop, use counter or sleep to pass some time between
    #update checks. At your designated point, check for updates.
    sleep(2)
    print('Updated Program, entering loop 2')
    sleep(2)
    print('Updated Program, entering loop 3')
    sleep(2)
    print('Updated Program, entering loop 4')
    sleep(2)
    print('Updated Program, entering loop 5')
    sleep(2)
    print('5 loops completed, checking for updates')
