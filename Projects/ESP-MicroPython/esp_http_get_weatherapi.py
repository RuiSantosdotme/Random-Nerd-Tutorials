# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details: https://RandomNerdTutorials.com/micropython-http-get-requests-esp32-esp8266/

import network
import time
import requests

# Wi-Fi credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

api_key = 'REPLACE_WITH_YOUR_API_KEY'
location ='REPLACE_WITH_YOUR_LOCATION'

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
        
        # Get specific weather data
        weather_description = weather['current']['condition']['text']
        print('Current weather: ', weather_description)
        
        # Temperature and humidity
        temperature_c = weather['current']['temp_c']
        temperature_f = weather['current']['temp_f']
        humidity = weather['current']['humidity']
        print(f'Temperature in Celsius: {temperature_c:.2f}')
        print(f'Temperature in Fahrenheit: {temperature_f:.2f}')
        print(f'Humidity (%): {humidity:.2f}')              
        
        # Precipitation
        precipitation = weather['current']['precip_mm']
        print(f'Precipitation in mm: {precipitation}')              
        
        # Wind
        wind_speed = weather['current']['wind_kph']
        print('Wind speed in kph:', wind_speed)

    except Exception as e:
        # Handle any exceptions during the request
        print('Error during request:', e)