# Complete project details at https://RandomNerdTutorials.com/micropython-mqtt-publish-bme680-esp32-esp8266/

import time
from umqttsimple import MQTTClient
import ubinascii
import machine
import micropython
import network
import esp
from bme680 import *
from machine import Pin, I2C

esp.osdebug(None)
import gc
gc.collect()

ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'
mqtt_server = 'XXX.XXX.XXX.XXX'
#EXAMPLE IP ADDRESS
#mqtt_server = '192.168.1.106'

client_id = ubinascii.hexlify(machine.unique_id())

topic_pub_temp = b'esp/bme680/temperature'
topic_pub_hum = b'esp/bme680/humidity'
topic_pub_pres = b'esp/bme680/pressure'
topic_pub_gas = b'esp/bme680/gas'

last_message = 0
message_interval = 5

station = network.WLAN(network.STA_IF)

station.active(True)
station.connect(ssid, password)

while station.isconnected() == False:
  pass

print('Connection successful')

# ESP32 - Pin assignment
#i2c = I2C(scl=Pin(22), sda=Pin(21))
# ESP8266 - Pin assignment
i2c = I2C(scl=Pin(5), sda=Pin(4))

bme = BME680_I2C(i2c=i2c)

def connect_mqtt():
  global client_id, mqtt_server
  client = MQTTClient(client_id, mqtt_server)
  #client = MQTTClient(client_id, mqtt_server, user=your_username, password=your_password)
  client.connect()
  print('Connected to %s MQTT broker' % (mqtt_server))
  return client

def restart_and_reconnect():
  print('Failed to connect to MQTT broker. Reconnecting...')
  time.sleep(10)
  machine.reset()

def read_bme_sensor():
  try:
    temp = (b'{:.2f}'.format(bme.temperature))
    #temp = (b'{0:.2f}'.format((bme.temperature) * (9/5) + 32))
    hum = (b'{:.2f}'.format(bme.humidity))
    pres = (b'{:.2f}'.format(bme.pressure))
    gas = (b'{:.2f}'.format(bme.gas/1000))
    
    return temp, hum, pres, gas
    #else:
    #  return('Invalid sensor readings.')
  except OSError as e:
    return('Failed to read sensor.')

try:
  client = connect_mqtt()
except OSError as e:
  restart_and_reconnect()

while True:
  try:
    if (time.time() - last_message) > message_interval:
      temp, hum, pres, gas = read_bme_sensor()
      print(temp)
      print(hum)
      print(pres)
      print(gas)
      client.publish(topic_pub_temp, temp)
      client.publish(topic_pub_hum, hum)      
      client.publish(topic_pub_pres, pres)
      client.publish(topic_pub_gas, gas)
      
      last_message = time.time()
  except OSError as e:
    restart_and_reconnect()
