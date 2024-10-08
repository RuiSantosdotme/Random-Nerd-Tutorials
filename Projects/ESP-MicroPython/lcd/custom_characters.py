# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-i2c-lcd-esp32-esp8266/

from machine import Pin, SoftI2C
from machine_i2c_lcd import I2cLcd
from time import sleep

# Define the LCD I2C address and dimensions
I2C_ADDR = 0x27
I2C_NUM_ROWS = 2
I2C_NUM_COLS = 16

# Initialize I2C and LCD objects
i2c = SoftI2C(sda=Pin(21), scl=Pin(22), freq=400000)

# for ESP8266, uncomment the following line
#i2c = SoftI2C(sda=Pin(4), scl=Pin(5), freq=400000)

lcd = I2cLcd(i2c, I2C_ADDR, I2C_NUM_ROWS, I2C_NUM_COLS)

# Create custom characters here: https://maxpromer.github.io/LCD-Character-Creator/
thermometer = bytearray([0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x1B, 0x1F, 0x0E])
lcd.custom_char(0, thermometer)

umbrella = bytearray([0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x014, 0x0C])
lcd.custom_char(1, umbrella)

heart = bytearray([0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00])
lcd.custom_char(2, heart)

try:
    lcd.putstr("Characters")
    lcd.move_to(0, 1)
    # Display thermometer
    lcd.putchar(chr(0))
    # Display umbrella
    lcd.move_to(2, 1)
    lcd.putchar(chr(1))
    # Display heart
    lcd.move_to(4, 1)
    lcd.putchar(chr(2))
 
except KeyboardInterrupt:
    # Turn off the display when the code is interrupted by the user
    print("Keyboard interrupt")
    lcd.backlight_off()
    lcd.display_off()