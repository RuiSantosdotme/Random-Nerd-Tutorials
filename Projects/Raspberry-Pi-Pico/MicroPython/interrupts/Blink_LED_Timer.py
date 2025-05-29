# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-interrupts-micropython/

from machine import Pin, Timer
from time import sleep

# LED pin
led_pin = 20
led = Pin(led_pin, Pin.OUT)

# Callback function for the timer
def toggle_led(timer):
    led.value(not led.value())  # Toggle the LED state (ON/OFF)

# Create a periodic timer
blink_timer = Timer()
blink_timer.init(mode=Timer.PERIODIC, period=500, callback=toggle_led)  # Timer repeats every half second

# Main loop (optional)
while True:
    print('Main Loop is running')
    sleep(2)