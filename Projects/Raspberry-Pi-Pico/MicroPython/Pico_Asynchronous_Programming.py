# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-raspberry-pi-pico-asynchronous-programming/

import asyncio
from machine import Pin

green_led_pin = 20
green_led = Pin(green_led_pin, Pin.OUT)
blue_led_pin = 19
blue_led = Pin(blue_led_pin, Pin.OUT)

# Define coroutine function
async def blink_green_led():
    while True:
        green_led.toggle()
        await asyncio.sleep(2) 

# Define coroutine function
async def blink_blue_led():
    while True:
        blue_led.toggle()
        await asyncio.sleep(0.5)

# Define the main function to run the event loop
async def main():
    # Create tasks for blinking two LEDs concurrently
    asyncio.create_task(blink_green_led())
    asyncio.create_task(blink_blue_led())

# Create and run the event loop
loop = asyncio.get_event_loop()  
loop.create_task(main())  # Create a task to run the main function
loop.run_forever()  # Run the event loop indefinitely
