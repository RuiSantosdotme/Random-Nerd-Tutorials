# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details: https://RandomNerdTutorials.com/raspberry-pi-pico-w-http-requests-micropython/
 
import network
import requests
from time import sleep

# Wi-Fi credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Request URL
url = 'https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd'

def init_wifi(ssid, password):# Init Wi-Fi Interface
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

if init_wifi(ssid, password):
    try:
        # Make the request
        response = requests.get(url)
        #Print the response code
        print('Response code: ', response.status_code)
        
        # Get response content
        bitcoin = response.json()
        # Close the request
        response.close()
        
        # Print bitcoin price
        bitcoin_price = bitcoin['bitcoin']['usd']
        print('Bitcoin price (USD): ', bitcoin_price)

    except Exception as e:
        # Handle any exceptions during the request
        print('Error during request:', e)

