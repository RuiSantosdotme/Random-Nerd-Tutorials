# Complete project details at https://RandomNerdTutorials.com/micropython-mqtt-publish-bme280-esp32-esp8266/

import time
from umqttsimple import MQTTClient
import ubinascii
import machine
import micropython
import network
import esp
import BME280
from machine import Pin, I2C

esp.osdebug(None)
import gc
gc.collect()

ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'
mqtt_server = '192.168.1.XXX'
#EXAMPLE IP ADDRESS
#mqtt_server = '192.168.1.106'

client_id = ubinascii.hexlify(machine.unique_id())

topic_pub_temp = b'esp/bme280/temperature'
topic_pub_hum = b'esp/bme280/humidity'
topic_pub_pres = b'esp/bme280/pressure'

last_message = 0
message_interval = 5

station = network.WLAN(network.STA_IF)

station.active(True)
station.connect(ssid, password)

while station.isconnected() == False:
  pass

print('Connection successful')

# ESP32 - Pin assignment
i2c = I2C(scl=Pin(22), sda=Pin(21), freq=10000)
# ESP8266 - Pin assignment
#i2c = I2C(scl=Pin(5), sda=Pin(4), freq=10000)
bme = BME280.BME280(i2c=i2c)

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
    temp = b'%s' % bme.temperature[:-1]
    #temp = (b'{0:3.1f},'.format((bme.read_temperature()/100) * (9/5) + 32))
    hum = b'%s' % bme.humidity[:-1]
    pres = b'%s'% bme.pressure[:-3]

    return temp, hum, pres
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
      temp, hum, pres = read_bme_sensor()
      print(temp)
      print(hum)
      print(pres)
      client.publish(topic_pub_temp, temp)
      client.publish(topic_pub_hum, hum)      
      client.publish(topic_pub_pres, pres)

      last_message = time.time()
  except OSError as e:
    restart_and_reconnect()
