"""
OTA updater for ESP32 running Micropython by David Flory.
Tutorial: https://randomnerdtutorials.com/esp32-esp8266-micropython-ota-updates/

A simple program to demonstrate OTA update functionality in conjunction with main.py
The program can be as simple or complicated as you like, but must contain these functions.
The variable 'url' should be the full web address to the location of the update file and the PHP script which
returns it. below is the PHP script, named 'get_ESP_data.php'.

<?php
    $file = $_GET['file'];
    $dir = getcwd();
    $file = $dir.'/'.$file;
    $myfile = fopen($file, "r") or die("FAIL");
    echo file_get_contents($file);
    fclose($myfile);
?>

The script reads the update file and echo's it back in response to the request made by the
'check_for_updates' function.

Here is the delete script for deleting the updates.
<?php
    $file = $_GET['file'];
    $dir = getcwd();
    $file = $dir.'/'.$file;
    unlink($file);
?>

"""
import urequests
from utime import sleep
#The URL to use to get updates, full path to the PHP script (Raspberry Pi IP Address). The update file must be in the same
#directory as the PHP script.
#REPLACE WITH YOUR RASPBERRY PI IP ADDRESS
upd_url="http://192.168.1.XXX/get_ESP_data.php?file=program.py"
del_url = "http://192.168.1.XXX/delete_ESP_data.php?file=program.py"

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
    #first delete any update files on server. The reasoning here is that if this program is an update we
    #do not want to download it again. If this program is not an update, it is unlikely there will be an
    #update present anyway. If we do not delete it, the program will keep on updating on every loop and rebooting.
    response = urequests.get(del_url)
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
    print('Tasks completed, entering loop 2')
    sleep(2)
    print('Tasks completed, entering loop 3')
    sleep(2)
    print('Tasks completed, entering loop 4')
    sleep(2)
    print('Tasks completed, entering loop 5')
    sleep(2)
    print('5 loops completed, checking for updates')
