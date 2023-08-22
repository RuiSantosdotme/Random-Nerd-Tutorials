# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-analog-inputs-micropython/
from machine import Pin, ADC
from time import sleep

pot = ADC(Pin(26))

while True:
  pot_value = pot.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
  print(pot_value)
  sleep(0.1)
