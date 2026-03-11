# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-tm1637-micropython/

import tm1637
from picozero import pico_temp_sensor
from machine import Pin, Timer
import time

# Initialize display (adjust pins if needed)
display = tm1637.TM1637(clk=Pin(21), dio=Pin(20))

# Set display brightness
display.brightness(7)

# Convert from celsius to fahrenheit
def celsius_to_fahrenheit(temp_celsius): 
    temp_fahrenheit = temp_celsius * (9/5) + 32 
    return temp_fahrenheit

# Get temperature readings
def getTemperature(Timer):
    # Declare these variables as globals
    global temperature_c, temperature_f
    
    # Reading and printing the internal temperature
    temperature_c = pico_temp_sensor.temp
    temperature_f = celsius_to_fahrenheit(temperature_c)

    print(f'Internal temperature in Celsius: {temperature_c:.2f}')
    print(f'Internal temperature in Fahrenheit: {temperature_f:.2f}')

# Get temperature for the first time
getTemperature(0)

# Create a periodic timer that gets new temperature readings
api_timer = Timer()
api_timer.init(mode=Timer.PERIODIC, period=60000, callback=getTemperature)

while True:
    display.temperature(round(temperature_c))
    time.sleep(5)
    display.temperature_f(round(temperature_f))
    time.sleep(5)
