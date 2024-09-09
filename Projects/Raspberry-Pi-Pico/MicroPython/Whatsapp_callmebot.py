# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-w-whatsapp-micropython/

import network
import requests
from time import sleep

# Wi-Fi credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Your phone number in international format (including the + sign)
phone_number = 'YOUR_PHONE_NUMER_INTERNATIONAL_FORMAT'
# Example: phone_number = '+351912345678'

# Your callmebot API key
api_key = 'CALLMEBOT_API_KEY'

# Init Wi-Fi Interface
def init_wifi(ssid, password):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    # Connect to your network
    wlan.connect(ssid, password)
    # Wait for Wi-Fi connection
    connection_timeout = 10
    while connection_timeout > 0:
        if wlan.status() >= 3:
            break
        connection_timeout -= 1
        print('Waiting for Wi-Fi connection...')
        sleep(1)
    # Check if connection is successful
    if wlan.status() != 3:
        return False
    else:
        print('Connection successful!')
        network_info = wlan.ifconfig()
        print('IP address:', network_info[0])
        return True

def send_message(phone_number, api_key, message):
    # Set the URL
    url = f'https://api.callmebot.com/whatsapp.php?phone={phone_number}&text={message}&apikey={api_key}'

    # Make the request
    response = requests.get(url)
    # check if it was successful
    if (response.status_code == 200):
        print('Success!')
    else:
        print('Error')
        print(response.text)
try: 
    # Connect to WiFi
    if init_wifi(ssid, password):
        # Send message to WhatsApp "Hello"
        # ENTER YOUR MESSAGE BELOW (URL ENCODED) https://www.urlencoder.io/
        message = 'Hello%20from%20the%20Raspberry%20Pi%20Pico%21' 
        send_message(phone_number, api_key, message)
except Exception as e:
    print('Error:', e)
