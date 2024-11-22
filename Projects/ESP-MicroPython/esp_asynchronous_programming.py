# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp32-esp8266-asynchronous-programming/

import asyncio
from machine import Pin

green_led_pin = 14
green_led = Pin(green_led_pin, Pin.OUT)
blue_led_pin = 12
blue_led = Pin(blue_led_pin, Pin.OUT)

# Define coroutine function
async def blink_green_led():
    while True:
        green_led.value(not green_led.value() )
        await asyncio.sleep(2) 

# Define coroutine function
async def blink_blue_led():
    while True:
        blue_led.value(not blue_led.value())
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
