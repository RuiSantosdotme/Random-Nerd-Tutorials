# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-motion-pir-micropython/

from machine import Pin
from time import sleep

pir_pin = Pin(22, Pin.IN)
led_pin = Pin (21, Pin.OUT)

while True:
    if pir_pin.value():
        print("Motion detected!")
        led_pin.value(1)
    sleep(1)
    led_pin.value(0)
