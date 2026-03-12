# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp32-esp8266-tm1637-display/
import network
import time
import requests
import tm1637
from machine import Pin, Timer

# Initialize display (adjust pins if needed)
display = tm1637.TM1637(clk=Pin(21), dio=Pin(20))

# Wi-Fi credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

api_key = 'REPLACE_WITH_YOUR_weatherapi'
location ='Oporto'   # Documentation https://www.weatherapi.com/docs/

# Request URL
url = f'https://api.weatherapi.com/v1/current.json?q={location}+&key={api_key}'

# Connect to network
def connect_wifi(ssid, password):
    # Connect to your network
    station = network.WLAN(network.STA_IF)
    station.active(True)
    station.connect(ssid, password)
    
    # Wait for connection
    timeout = 10
    while not station.isconnected() and timeout > 0:
        time.sleep(1)
        timeout -= 1
    
    if station.isconnected():
        print('Connection successful')
        print(station.ifconfig())
        return True
    else:
        print('Connection failed - timeout reached')
        return False
    
def getWeather(timer):
    if connect_wifi(ssid, password):
        try:
            # Make the request
            response = requests.get(url)
            #Print the response code
            print('Response code: ', response.status_code)
            
            # Get response content
            weather = response.json()
            # Close the request
            response.close()
            
            # Print bulk weather data
            print('Weather JSON: ', weather)
            
            # Declare these variables as globals
            global weather_description, temperature_c, temperature_f

            # Get specific weather data
            weather_description = weather['current']['condition']['text']
            print('Current weather: ', weather_description)
            
            # Temperature and humidity
            temperature_c = weather['current']['temp_c']
            temperature_f = weather['current']['temp_f']
            print(f'Temperature in Celsius: {temperature_c:.2f}')
            print(f'Temperature in Fahrenheit: {temperature_f:.2f}')            

        except Exception as e:
            # Handle any exceptions during the request
            print('Error during request:', e)
            display.show('erro')
            
# Get Weather for the first time
getWeather(0)

# Create a periodic timer that gets new data from the API
api_timer = Timer()
api_timer.init(mode=Timer.PERIODIC, period=3600000, callback=getWeather)

while True:
    display.scroll(weather_description)
    time.sleep(1)
    display.temperature(round(temperature_c))
    time.sleep(5)
    display.temperature_f(round(temperature_f))
    time.sleep(5)
