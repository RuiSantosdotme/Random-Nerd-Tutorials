# Complete project details at https://RandomNerdTutorials.com
import machine
from machine import Pin, I2C

import BME280
import network
import urequests
from time import sleep

import esp
esp.osdebug(None)

import gc
gc.collect()

ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

api_key = 'REPLACE_WITH_YOUR_IFTTT_API_KEY'

ms_sleep_time = 600000

station = network.WLAN(network.STA_IF)

station.active(True)
station.connect(ssid, password)

while station.isconnected() == False:
  pass

print('Connection successful')
print(station.ifconfig())

def deep_sleep(msecs) :
  # configure RTC.ALARM0 to be able to wake the device
  rtc = machine.RTC()
  rtc.irq(trigger=rtc.ALARM0, wake=machine.DEEPSLEEP)

  # set RTC.ALARM0 to fire after X milliseconds (waking the device)
  rtc.alarm(rtc.ALARM0, msecs)

  # put the device to sleep
  machine.deepsleep()

# ESP8266 - Pin assignement
i2c = I2C(scl=Pin(5),sda=Pin(4), freq=10000)

# ESP32 - Pin assignement
#i2c = I2C(scl=Pin(22),sda=Pin(21), freq=10000)

try:
  bme = BME280.BME280(i2c=i2c)
  temp = bme.temperature
  hum = bme.humidity
  pres = bme.pressure

  # uncomment for temperature in Fahrenheit
  #temp = (bme.read_temperature()/100) * (9/5) + 32
  #temp = str(round(temp, 2)) + 'F'

  sensor_readings = {'value1':temp[:-1], 'value2':hum[:-1], 'value3':pres[:-3]}
  print(sensor_readings)

  request_headers = {'Content-Type': 'application/json'}

  request = urequests.post(
    'http://maker.ifttt.com/trigger/bme280/with/key/' + api_key,
    json=sensor_readings,
    headers=request_headers)
  print(request.text)
  request.close()

except OSError as e:
  print('Failed to read/publish sensor readings.')

sleep(10)

#ESP8266
deep_sleep(ms_sleep_time)

#ESP32
#machine.deepsleep(ms_sleep_time)
