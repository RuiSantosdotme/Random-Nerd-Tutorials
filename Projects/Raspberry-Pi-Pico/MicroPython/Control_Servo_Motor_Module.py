# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-servo-motor-micropython/

from servo import Servo
from time import sleep

# Create a Servo object on pin 0
servo=Servo(pin=0)

try:
    while True:
        #Servo at 0 degrees
        servo.move(0)
        sleep(2)
        #Servo at 90 degrees
        servo.move(90)
        sleep(2)
        #Servo at 180 degrees
        servo.move(180)
        sleep(2)
        
except KeyboardInterrupt:
    print("Keyboard interrupt")
    # Turn off PWM 
    servo.stop()