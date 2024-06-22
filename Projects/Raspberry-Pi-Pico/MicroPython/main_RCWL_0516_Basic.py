# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-rcwl-0516-micropython/

from machine import Pin
import time

# Pin numbers for LED and sensor
led_pin = 21
sensor_pin = 22

# Set the LED pin as an output and the sensor pin as an input
led = Pin(led_pin, Pin.OUT)
sensor = Pin(sensor_pin, Pin.IN)

# Initialize state variables
state = 0
val = 0

while True:
    val = sensor.value()  # read sensor value

    if val == 1:  # check if the sensor is HIGH
        led.on()  # turn LED ON

        if state == 0:
            print("Motion detected!")
            state = 1  # update variable state to HIGH
    else:
        led.off()  # turn LED OFF

        if state == 1:
            print("Motion stopped!")
            state = 0  # update variable state to LOW

    # Add a small delay to avoid constant checking of the sensor
    time.sleep(0.1)
