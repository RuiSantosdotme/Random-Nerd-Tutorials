#!/usr/bin/python
 
#import libraries
import smtplib

import Adafruit_BBIO.GPIO as GPIO
import time
 
#create a variable called PIR, which refers to the P8_11 pin
PIR = "P8_11"
 
#initialize the pin as an INPUT
GPIO.setup(PIR, GPIO.IN)
GPIO.add_event_detect(PIR, GPIO.RISING)

def send_email():
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.ehlo()
    server.starttls()
    server.ehlo()
    my_email = "REPLACE_WITH_YOUR_EMAIL@gmail.com
    my_password = "REPLACE_WITH_YOUR_PASSWORD"
    destination = "REPLACE_WITH_YOUR_EMAIL@gmail.com"
    text = "Motion has been detected at your house!"
    
    server.login(my_email, my_password)
    server.sendmail(my_email, destination, text)
    server.quit()
    print("Your email has been sent!")
       
#loop forever
while True:
    #sends an email when motion has been detected
    if GPIO.event_detected(PIR):
        send_email() 
    time.sleep(0.05) #loop every 50 miliseconds to not overburden the CPU
