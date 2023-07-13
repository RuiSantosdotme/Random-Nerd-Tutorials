# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-motion-email-python/

#import necessary libraries
from gpiozero import LED, Button, MotionSensor
import smtplib
from email.message import EmailMessage
from signal import pause
from time import sleep

#create objects to refer to the LED, the button, and the PIR sensor
led_status = LED(14)
button = Button(4)
pir = MotionSensor(18)

#replace the next three lines with your credentials
from_email_addr = "REPLACE_WITH_SENDER_EMAIL_ADDRESS"
from_email_password = "REPLACE_WITH_SENDER_APP_PASSWORD"
to_email_addr = "REPLACE_WIH_RECIPIENT_EMAIL_ADDRESS"
email_subject = "[WARNING!] Intruder Alert!"
email_body = "Motion was detected in your room!"

#control variables
motion_sensor_status = False
email_sent = False

#arm or disarm the PIR sensor
def arm_motion_sensor():
    global email_sent
    global motion_sensor_status

    if motion_sensor_status == True:
       motion_sensor_status = False
       led_status.off()
       print("Motion Sensor OFF")
    else:
        motion_sensor_status = True
        email_sent = False
        led_status.on()
        print("Motion Sensor ON")

#send email when motion is detected and the PIR sensor is armed
def send_email():
    global email_sent
    global motion_sensor_status

    if(motion_sensor_status == True and email_sent == False):
        print("Motion Detected")
        #create a message object
        msg = EmailMessage()
        #set the email body
        msg.set_content(email_body)
        #set sender and recipient
        msg['From'] = from_email_addr
        msg['To'] = to_email_addr
        #set your email subject
        msg['Subject'] = email_subject
        #connect to server and send email
        #edit this line with your provider's SMTP server details
        server = smtplib.SMTP('smtp.gmail.com', 587)
        #comment out this line if your provider doesn't use TLS
        server.starttls()
        server.login(from_email_addr, from_email_password)
        server.send_message(msg)
        server.quit()
        email_sent = True
        print('Email sent')
        sleep(5)
        email_sent = False

#assign a function that runs when the button is pressed
button.when_pressed = arm_motion_sensor

#assign a function that runs when motion is detected
pir.when_motion = send_email

pause()
