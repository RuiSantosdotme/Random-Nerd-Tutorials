# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-internal-temperature-micropython/

from machine import ADC

# Internal temperature sensor is connected to ADC channel 4
temp_sensor = ADC(4)

def read_internal_temperature():
    # Read the raw ADC value
    adc_value = temp_sensor.read_u16()

    # Convert ADC value to voltage
    voltage = adc_value * (3.3 / 65535.0)

    # Temperature calculation based on sensor characteristics
    temperature_celsius = 27 - (voltage - 0.706) / 0.001721

    return temperature_celsius

def celsius_to_fahrenheit(temp_celsius): 
    temp_fahrenheit = temp_celsius * (9/5) + 32 
    return temp_fahrenheit

# Reading and printing the internal temperature
temperatureC = read_internal_temperature()
temperatureF = celsius_to_fahrenheit(temperatureC)
print("Internal Temperature:", temperatureC, "°C")
print("Internal Temperature:", temperatureF, "°F")
