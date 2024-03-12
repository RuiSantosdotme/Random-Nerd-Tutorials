# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-internal-temperature-micropython/

from time import sleep
from picozero import pico_temp_sensor

# Convert from celsius to fahrenheit
def celsius_to_fahrenheit(temp_celsius): 
    temp_fahrenheit = temp_celsius * (9/5) + 32 
    return temp_fahrenheit

while True:
    # Reading and printing the internal temperature
    temperatureC = pico_temp_sensor.temp
    temperatureF = celsius_to_fahrenheit(temperatureC)

    print("Internal Temperature:", temperatureC, "°C")
    print("Internal Temperature:", temperatureF, "°F")
    
    # Wait one second between each reading
    sleep(1)
