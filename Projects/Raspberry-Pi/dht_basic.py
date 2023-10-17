# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-motion-email-python/

import Adafruit_DHT
import time

# comment and uncomment the lines below depending on your sensor
sensor = Adafruit_DHT.DHT22
# sensor = Adafruit_DHT.DHT11

# DHT pin connects to GPIO 4
sensor_pin = 4

while True:
    humidity, temperature = Adafruit_DHT.read_retry(sensor, sensor_pin)  # Use read_retry to retry in case of failure
    if humidity is not None and temperature is not None:
        # Uncomment the line below to convert to Fahrenheit
        temperatureF = temperature * 9/5.0 + 32
        print("Temp={0:0.1f}ºC, Temp={1:0.1f}ºF, Humidity={2:0.1f}%".format(temperature, temperatureF, humidity))
    else:
        print("Sensor failure. Check wiring.")
    time.sleep(3)
