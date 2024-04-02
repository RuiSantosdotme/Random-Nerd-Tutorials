# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-motion-pir-micropython/

from machine import Pin
from time import sleep

# Define the PIR sensor input pin
pir_pin = Pin(22, Pin.IN)

# Define the LED pin (optional, for visual feedback)
led_pin = Pin(21, Pin.OUT)

# Flags to indicate motion detection state
motion_detected = False

motion_stopped_printed = False

# Callback function to handle motion detection
def pir_interrupt(pin):
    global motion_detected
    if pin.value() == 1:  # Rising edge (motion detected)
        motion_detected = True
        led_pin.value(1)  # Turn on the LED
    else:  # Falling edge (motion stopped)
        motion_detected = False
        led_pin.value(0)  # Turn off the LED

# Configure the interrupt on the PIR pin for both rising and falling edges
pir_pin.irq(trigger=(Pin.IRQ_RISING | Pin.IRQ_FALLING), handler=pir_interrupt)

try:
    while True:
        if motion_detected and not motion_stopped_printed:
            print("Motion detected!")
            motion_stopped_printed = True  # Set the flag to indicate motion detected

        elif not motion_detected and motion_stopped_printed:
            print("Motion stopped")
            motion_stopped_printed = False  # Reset the flag

        sleep(0.1)  # Main loop delay

except KeyboardInterrupt:
    print("Keyboard interrupt")
    pir_pin.irq(trigger=0)  # Disable the interrupt
    led_pin.value(0)  # Turn off the LED
    
except Exception as e:
    print("An unexpected exception occurred:", e)	
