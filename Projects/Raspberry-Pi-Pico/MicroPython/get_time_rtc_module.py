# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ds1307-rtc-micropython/

import time
import urtc
from machine import SoftI2C, Pin
import ssd1306

days_of_week = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday']

# Initialize RTC Module
i2c_rtc = SoftI2C(scl=Pin(5), sda=Pin(4))
rtc = urtc.DS1307(i2c_rtc)

# Initialize OLED 
oled_width = 128
oled_height = 64
i2c_oled = SoftI2C(scl=Pin(0), sda=Pin(1))
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c_oled)

while True:
    # Get current time from the RTC
    current_datetime = rtc.datetime()

    # Format the date and time as strings
    formatted_date = '{:02d}-{:02d}-{:04d}'.format(current_datetime.day, current_datetime.month, current_datetime.year)
    formatted_time = '{:02d}:{:02d}:{:02d}'.format(current_datetime.hour, current_datetime.minute, current_datetime.second)
    formatted_day_week = days_of_week[current_datetime.weekday]

    # Clear the OLED display
    oled.fill(0)

    # Display the formatted date and time
    oled.text('Date: ' + formatted_day_week, 0, 0)
    oled.text(formatted_date, 0, 16)
    oled.text('Time: ' + formatted_time, 0, 32)
    oled.show()

    # Print the formatted date and time to the shell
    print('Formatted date:', formatted_date)
    print('Formatted time:', formatted_time)

    # Wait for 1 second
    time.sleep(1)
