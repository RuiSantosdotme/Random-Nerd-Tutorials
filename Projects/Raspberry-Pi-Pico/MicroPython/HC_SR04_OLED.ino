# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-hc-sr04-micropython/

from machine import Pin, I2C
import ssd1306
from hcsr04 import HCSR04
from time import sleep

# Initialize I2C for SSD1306 OLED (Pico: SCL = GPIO 5, SDA = GPIO 4 for I2C1)
i2c = I2C(scl=Pin(5), sda=Pin(4))

# Initialize HC-SR04 ultrasonic sensor (trigger = GPIO 27, echo = GPIO 28)
sensor = HCSR04(trigger_pin=27, echo_pin=28, echo_timeout_us=10000)

# Initialize SSD1306 OLED (128x64)
oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

while True:
    try:
        # Measure distance in centimeters
        distance_cm = sensor.distance_cm()
        # Convert distance from centimeters to inches
        #distance_inch = distance_cm * 0.393701
        
        # Print to shell
        print('Distance: {:.2f} cm'.format(distance_cm))
        
        # Clear OLED and display distance in cm
        oled.fill(0)
        oled.text("Distance:", 0, 10)
        oled.text("{:.2f} cm".format(distance_cm), 0, 30)
        oled.show()
        
    except OSError as e:
        # Handle sensor errors (e.g., out of range)
        print('Error:', e)
        oled.fill(0)
        oled.text("Error:", 0, 10)
        oled.text("Out of range", 0, 30)
        oled.show()
    
    # Wait 1 second before next measurement
    sleep(1)
