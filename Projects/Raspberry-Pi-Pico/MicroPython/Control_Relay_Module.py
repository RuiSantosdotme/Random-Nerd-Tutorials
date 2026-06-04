# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-relay-micropython/

from machine import Pin
from time import sleep

relay = Pin(0, Pin.OUT)

while True:
    # RELAY ON
    relay.value(0)
    sleep(10)   
    # RELAY OFF
    relay.value(1)
    sleep(10)
