# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ssd1306-oled-micropython/

from machine import Pin, SoftI2C
import ssd1306
from time import sleep

try:
    # You can choose any other combination of I2C pins
    i2c = SoftI2C(scl=Pin(5), sda=Pin(4))

    # Set up the OLED display parameters
    oled_width = 128
    oled_height = 64
    oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

    # Display 'Hello, World' messages on different lines
    oled.text('Hello, World 1!', 0, 0)
    oled.text('Hello, World 2!', 0, 10)
    oled.text('Hello, World 3!', 0, 20)

    # Update the OLED display to show the messages
    oled.show()

    # Keep the program running for a while (for demonstration purposes)
    sleep(5)

except KeyboardInterrupt:
    # Handle keyboard interrupt (Ctrl+C)
    print("Program interrupted by user")

finally:
    # Turn off the OLED display before exiting the program
    oled.fill(0)
    oled.show()
