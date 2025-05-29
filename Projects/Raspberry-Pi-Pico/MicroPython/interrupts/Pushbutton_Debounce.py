# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-interrupts-micropython/

from machine import Pin, Timer

led = Pin(20, Pin.OUT)
button = Pin(21, Pin.IN, Pin.PULL_DOWN)
counter = 0  # Initialize the button press count
debounce_timer = None

def button_pressed(pin):
    global counter, debounce_timer  # Declare variables as global

    if debounce_timer is None:
        counter += 1
        print("Button Pressed! Count: ", counter)
        
        # Toggle the LED on each button press
        led.toggle()

        # Start a timer for debounce period (e.g., 200 milliseconds)
        debounce_timer = Timer()
        debounce_timer.init(mode=Timer.ONE_SHOT, period=200, callback=debounce_callback)

def debounce_callback(timer):
    global debounce_timer
    debounce_timer = None

# Attach the interrupt to the button's rising edge
button.irq(trigger=Pin.IRQ_RISING, handler=button_pressed)