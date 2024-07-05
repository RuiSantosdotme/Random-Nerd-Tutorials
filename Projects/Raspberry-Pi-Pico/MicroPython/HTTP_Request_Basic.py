# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details: https://RandomNerdTutorials.com/raspberry-pi-pico-w-http-requests-micropython/
 
import network
import requests

# Wi-Fi credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Connect to network
wlan = network.WLAN(network.STA_IF)
wlan.active(True)

# Connect to your network
wlan.connect(ssid, password)

# Make GET request
response = requests.get("http://www.google.com")
# Get response code
response_code = response.status_code
# Get response content
response_content = response.content

# Print results
print('Response code: ', response_code)
print('Response content:', response_content)