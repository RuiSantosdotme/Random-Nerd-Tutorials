# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ds3231-rtc-micropython/

import time
import urtc
from machine import Pin, I2C

# Pin setup for SQW pin and LED
CLOCK_INTERRUPT_PIN = 3  # Adjust to your specific GPIO pin for SQW
LED_PIN = 2  # Adjust to your specific GPIO pin for the LED

# Initialize RTC (connected to I2C)
i2c = I2C(0, scl=Pin(5), sda=Pin(4))
rtc = urtc.DS3231(i2c)

# Setup GPIO for SQW and LED
sqw_pin = Pin(CLOCK_INTERRUPT_PIN, Pin.IN, Pin.PULL_UP)
led_pin = Pin(LED_PIN, Pin.OUT)
led_pin.off()

# Alarm times (year, month, day, weekday, hour, minute, second, millisecond)
alarm1_time = urtc.datetime_tuple(2025, 01, 04, None, 12, 32, 00, 0)  # Alarm 1 uses full datetime
alarm2_time = urtc.datetime_tuple(2025, 01, 04, None, 12, 35, 00, 0)   # Alarm 2 uses day, hour, minute, weekday

# Print the current time from the RTC
def print_current_time():
    now = rtc.datetime()
    formatted_time = f"{now.year}-{now.month:02}-{now.day:02} {now.hour:02}:{now.minute:02}:{now.second:02}"
    print(f"Current time: {formatted_time}")


#Callback for handling alarm interrupt.
def on_alarm(pin):
    print("Interrupt detected.")
    
    if rtc.alarm(alarm=0):  # Check if Alarm 1 triggered
        print("Alarm 1 triggered.")
        rtc.alarm(False, alarm=0)  # Clear Alarm 1 flag
        led_pin.value(not led_pin.value())  # Toggle LED for Alarm 1
    
    if rtc.alarm(alarm=1):  # Check if Alarm 2 triggered
        print("Alarm 2 triggered.")
        rtc.alarm(False, alarm=1)  # Clear Alarm 2 flag
        led_pin.value(not led_pin.value())  # Toggle LED for Alarm 2

# Setup alarms on the DS3231
def setup_alarms():
    # Clear any existing alarms
    rtc.alarm(False, 0)
    rtc.alarm(False, 1)
    rtc.no_interrupt()

    # Set the desired alarm times
    rtc.alarm_time(alarm1_time, 0)  # Alarm 1
    rtc.alarm_time(alarm2_time, 1)  # Alarm 2

    # Enable interrupts for the alarms
    rtc.interrupt(0)
    rtc.interrupt(1)
    print("Alarms set successfully.")


# Attach the interrupt callback
sqw_pin.irq(trigger=Pin.IRQ_FALLING, handler=on_alarm)

try:
    # Sync time to compile time if RTC lost power
    if rtc.lost_power():
        initial_time_tuple = time.localtime()  # tuple (MicroPython)
        initial_time_seconds = time.mktime(initial_time_tuple)  # local time in seconds
        initial_time = urtc.seconds2tuple(initial_time_seconds)  # Convert to tuple compatible with the library
        rtc.datetime(initial_time)  # Sync the RTC

    print("RTC initialized. Setting alarms...")
    setup_alarms()

    while True:
        print_current_time()
        time.sleep(5)
except KeyboardInterrupt:
    print("Exiting program.")
