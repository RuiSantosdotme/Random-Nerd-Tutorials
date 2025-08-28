# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-hc-sr04-micropython/

from machine import Pin
import time
from hcsr04 import HCSR04

# Initialize the HC-SR04 sensor with trigger on GPIO 27 and echo on GPIO 28
sensor = HCSR04(trigger_pin=27, echo_pin=28, echo_timeout_us=30000)

while True:
    try:
        # Measure distance in centimeters
        #distance_cm = sensor.distance_cm()
        # Convert distance from centimeters to inches
        #distance_inch = distance_cm * 0.393701
        #print('Distance: {:.2f} cm'.format(distance_cm))
        
        # Measure distance in millimeters
        distance_mm = sensor.distance_mm()
        print('Distance: {} mm'.format(distance_mm))

    except OSError as e:
        print('Error:', e)
    
    # Wait 1 second before the next measurement
    time.sleep(1) 
