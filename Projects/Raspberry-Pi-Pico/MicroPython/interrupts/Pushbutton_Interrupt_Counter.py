# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/

from machine import Pin

button = Pin(21, Pin.IN, Pin.PULL_DOWN)
counter = 0  # Initialize the button press count

def button_pressed(pin):
    global counter # Declare variable as global
    counter +=1
    print("Button Pressed! Count: ", counter)

# Attach the interrupt to the button's rising edge
button.irq(trigger=Pin.IRQ_RISING, handler=button_pressed)