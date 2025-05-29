# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-interrupts-micropython/

from machine import Pin

button = Pin(21, Pin.IN, Pin.PULL_DOWN)

def button_pressed(pin):
    print("Button Pressed!")

# Attach the interrupt to the button's rising edge
button.irq(trigger=Pin.IRQ_RISING, handler=button_pressed)