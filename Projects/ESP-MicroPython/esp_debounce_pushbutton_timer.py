# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-timer-interrupts-ep32-esp8266/
 
from machine import Pin, Timer
import time

led = Pin(13, Pin.OUT)
button = Pin(12, Pin.IN, Pin.PULL_UP)
counter = 0  # Initialize the button press count
debounce_timer = None

def button_pressed(pin):
    global counter, debounce_timer  # Declare variables as global

    if debounce_timer is None:
        counter += 1
        print("Button Pressed! Count: ", counter)
        
        # Toggle the LED on each button press
        led.value(not led.value())

        # Start a timer for debounce period (e.g., 200 milliseconds)
        debounce_timer = Timer(1)
        debounce_timer.init(mode=Timer.ONE_SHOT, period=200, callback=debounce_callback)

def debounce_callback(timer):
    global debounce_timer
    debounce_timer = None

# Attach the interrupt to the button's rising edge
button.irq(trigger=Pin.IRQ_RISING, handler=button_pressed)

try:
    # Main loop (optional)
    while True:
        print("Loop is running")
        time.sleep(5)
except KeyboardInterrupt:
    # Keyboard interrupt occurred, deinitialize the timer
    debounce_timer.deinit()
    # Turn off the LED
    led.value(0)