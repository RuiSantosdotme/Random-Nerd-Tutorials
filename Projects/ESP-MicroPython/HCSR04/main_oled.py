# Complete project details at https://RandomNerdTutorials.com/micropython-hc-sr04-ultrasonic-esp32-esp8266/
from machine import Pin, I2C
import ssd1306
from hcsr04 import HCSR04
from time import sleep

# ESP32 Pin assignment 
i2c = I2C(scl=Pin(22), sda=Pin(21))
sensor = HCSR04(trigger_pin=5, echo_pin=18, echo_timeout_us=10000)

# ESP8266 Pin assignment
#i2c = I2C(scl=Pin(5), sda=Pin(4))
#sensor = HCSR04(trigger_pin=12, echo_pin=14, echo_timeout_us=10000)

oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

while True:
  oled.fill(0)
  #oled.show()
  distance = sensor.distance_mm()
  print('Distance:', distance, 'mm')
  oled.text("Distance (mm)", 0, 15)
  oled.text(str(distance), 0, 35)
  oled.show()
  sleep(1)