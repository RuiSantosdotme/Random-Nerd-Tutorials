# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-dht11-dht22-micropython/

from machine import Pin
from time import sleep
import dht 

sensor = dht.DHT22(Pin(22))
#sensor = dht.DHT11(Pin(22))

while True:
  try:
    sleep(2)
    sensor.measure()
    temp = sensor.temperature()
    hum = sensor.humidity()
    temp_f = temp * (9/5) + 32.0
    print('Temperature: %3.1f C' %temp)
    print('Temperature: %3.1f F' %temp_f)
    print('Humidity: %3.1f %%' %hum)
  except OSError as e:
    print('Failed to read sensor.')
