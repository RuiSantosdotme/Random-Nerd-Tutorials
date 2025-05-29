# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-interrupts-micropython/

from machine import Pin, Timer
import time

# PIR sensor pin
pir_pin = 21
# LED pin 
led_pin = 20

# Set up LED and PIR sensor
led = Pin(led_pin, Pin.OUT)
pir = Pin(pir_pin, Pin.IN)

# Create timer
motion_timer = Timer()

# Create timer
motion = False
motion_printed = False

def handle_motion(pin):
    global motion
    motion = True
    motion_timer.init(mode=Timer.ONE_SHOT, period=20000, callback=turn_off_led)

def turn_off_led(timer):
    global motion, motion_printed
    led.off()  # Turn off the LED
    print('Motion stopped!')
    motion = False
    motion_printed = False

# Attach interrupt to the PIR motion sensor
pir.irq(trigger=Pin.IRQ_RISING, handler=handle_motion)

while True:
    if motion and not motion_printed:
        print('Motion detected!')
        led.on()  # Turn on the LED
        motion_printed = True
    elif not motion:
        time.sleep(0.1)
        # Other tasks that you might need to do in the loop