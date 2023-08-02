#  Rui Santos
#  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-outputs-inputs-micropython/

from machine import Pin
from time import sleep

led = Pin(20, Pin.OUT)
button = Pin(21, Pin.IN, Pin.PULL_DOWN)

while True:
  led.value(button.value())
  sleep(0.1)
  print(button.value())