# Complete project details at https://RandomNerdTutorials.com

from machine import Pin, SoftI2C
import ssd1306
from time import sleep

# Start I2C Communication SCL = 4 and SDA = 5 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
i2c = SoftI2C(scl=Pin(4), sda=Pin(5))

oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

oled.text('Hello, World 1!', 0, 0)
oled.text('Hello, World 2!', 0, 10)
oled.text('Hello, World 3!', 0, 20)
        
oled.show()
