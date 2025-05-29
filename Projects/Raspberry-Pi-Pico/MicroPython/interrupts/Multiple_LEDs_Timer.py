# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-interrupts-micropython/

from machine import Pin, Timer
from time import sleep

# LEDs
green_led_pin = 19
green_led = Pin(green_led_pin, Pin.OUT)
blue_led_pin = 20
blue_led = Pin(blue_led_pin, Pin.OUT)

# Callback function for the green timer
def toggle_green_led(timer):
    green_led.toggle()  # Toggle the LED state (ON/OFF)
    print('Green LED is: ', green_led.value())

# Callback function for the blue timer
def toggle_blue_led(timer):
    blue_led.toggle()  # Toggle the LED state (ON/OFF)
    print('Blue LED is: ', blue_led.value())

# Create periodic timers
green_timer = Timer()
blue_timer = Timer()

# Init the timers
green_timer.init(mode=Timer.PERIODIC, period=500, callback=toggle_green_led)  # Timer repeats every 0.5 second
blue_timer.init(mode=Timer.PERIODIC, period=2000, callback=toggle_blue_led)  # Timer repeats every 2 seconds

# Main loop (optional)
while True:
    print('Main Loop is running')
    sleep(2)