# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-i2c-lcd-display-micropython/

from machine import Pin, SoftI2C
from pico_i2c_lcd import I2cLcd
from time import sleep

# Define the LCD I2C address and dimensions
I2C_ADDR = 0x27
I2C_NUM_ROWS = 2
I2C_NUM_COLS = 16

# Initialize I2C and LCD objects
i2c = SoftI2C(sda=Pin(4), scl=Pin(5), freq=400000)
lcd = I2cLcd(i2c, I2C_ADDR, I2C_NUM_ROWS, I2C_NUM_COLS)

def scroll_message(message, delay=0.3):
    # Add spaces to the beginning of the message to make it appear from the right
    message = " " * I2C_NUM_COLS + message + " "
    # Scroll through the message
    for i in range(len(message) - I2C_NUM_COLS + 1):
        lcd.move_to(0, 0)
        lcd.putstr(message[i:i + I2C_NUM_COLS])
        sleep(delay)

try:
    lcd.putstr("Testing scroll!")
    sleep(2)

    # Define the message to be scrolled
    message_scroll = "This is a scrolling message with more than 16 characters"

    while True:
        # Scroll the message on the LCD
        lcd.clear()
        scroll_message(message_scroll)

except KeyboardInterrupt:
    # Turn off the display when the code is interrupted by the user
    print("Keyboard interrupt")
    lcd.backlight_off()
    lcd.display_off()
