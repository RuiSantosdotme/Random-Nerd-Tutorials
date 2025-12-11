# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details: https://RandomNerdTutorials.com/micropython-http-get-requests-esp32-esp8266/
 
import network
import requests
import time

# Wi-Fi credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

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
        print('Connection failed. Timeout reached')
        return False

if connect_wifi(ssid, password):    
    # Make GET request
    try:
        response = requests.get("https://gist.githubusercontent.com/RuiSantosdotme/9e834367f02c198bc6474938e4e3bb0d/raw/de66b26a95fc2d0b7620a43b49e57607bd4348e3/url.txt")
        # Get response code
        response_code = response.status_code
        # Get response content
        response_content = response.content
        # Print results
        print('Response code: ', response_code)
        print('Response content:', response_content)
    except Exception as e:
        print('An error occurred during the request:', str(e))
else:
    print('Failed to connect to WiFi')