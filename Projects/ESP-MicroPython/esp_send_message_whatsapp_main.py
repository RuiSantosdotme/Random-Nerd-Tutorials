# Complete project details at https://RandomNerdTutorials.com/micropython-whatsapp-esp32-esp826/

try:
  import urequests as requests
except:
  import requests
  
import network

import esp
esp.osdebug(None)

import gc
gc.collect()

#Your network credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_SSID'

#Your phone number in international format
phone_number = 'YOUR_PHONE_NUMER_INTERNATIONAL_FORMAT'
#Your callmebot API key
api_key = 'CALLMEBOT_API_KEY'

def connect_wifi(ssid, password):
  #Connect to your network
  station = network.WLAN(network.STA_IF)
  station.active(True)
  station.connect(ssid, password)
  while station.isconnected() == False:
    pass
  print('Connection successful')
  print(station.ifconfig())

def send_message(phone_number, api_key, message):
  #set your host URL
  url = 'https://api.callmebot.com/whatsapp.php?phone='+phone_number+'&text='+message+'&apikey='+api_key

  #make the request
  response = requests.get(url)
  #check if it was successful
  if response.status_code == 200:
    print('Success!')
  else:
    print('Error')
    print(response.text)

# Connect to WiFi
connect_wifi(ssid, password)
# Send message to WhatsApp "Hello"
message = 'Hello%20from%20ESP32%20%28micropython%29' #YOUR MESSAGE HERE (URL ENCODED)https://www.urlencoder.io/ 
send_message(phone_number, api_key, message)