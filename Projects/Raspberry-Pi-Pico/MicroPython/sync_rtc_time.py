# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ds1307-rtc-micropython/

import time
import urtc
from machine import I2C, Pin

days_of_week = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday']

# Initialize RTC (connected to I2C)
i2c = I2C(0, scl=Pin(5), sda=Pin(4))
rtc = urtc.DS1307(i2c)

# Set the current time using a specified time tuple
# Time tupple: (year, month, day, day of week, hour, minute, seconds, milliseconds)
#initial_time = (2024, 1, 30, 1, 12, 30, 0, 0)

# Or get the local time from the system
initial_time_tuple = time.localtime() #tuple (microPython)
initial_time_seconds = time.mktime(initial_time_tuple) # local time in seconds

# Convert to tuple compatible with the library
initial_time = urtc.seconds2tuple(initial_time_seconds)

# Sync the RTC
rtc.datetime(initial_time)

while True:
    current_datetime = rtc.datetime()
    print('Current date and time:')
    print('Year:', current_datetime.year)
    print('Month:', current_datetime.month)
    print('Day:', current_datetime.day)
    print('Hour:', current_datetime.hour)
    print('Minute:', current_datetime.minute)
    print('Second:', current_datetime.second)
    print('Day of the Week:', days_of_week[current_datetime.weekday])

    time.sleep(1)
