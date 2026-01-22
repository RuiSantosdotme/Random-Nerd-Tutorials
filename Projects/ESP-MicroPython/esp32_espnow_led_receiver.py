# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp-now-esp32-one-to-many/

import network
import aioespnow
import asyncio
from machine import Pin

# Initialize Wi-Fi in station mode
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.config(channel=1)  # Match sender's channel
sta.disconnect()

# Initialize AIOESPNow
e = aioespnow.AIOESPNow()
try:
    e.active(True)
except OSError as err:
    print("Failed to initialize AIOESPNow:", err)
    raise

# Sender's MAC address
sender_mac = b'\xff\xff\xff\xff\xff\xff'  # You need to replace with actual sender MAC address

# Add sender as peer for more reliability
try:
    e.add_peer(sender_mac)
    print(f"Added sender peer: {sender_mac}")
except OSError as err:
    print(f"Failed to add sender peer: {err}")
    raise

# Initialize LED
led = Pin(2, Pin.OUT)
led.value(0)

# Async function to receive messages and control LED
async def receive_messages(e):
    while True:
        try:
            async for mac, msg in e:
                message = msg.decode()
                print(f"Received from {mac.hex()}: {message}")
                if message == "LED_ON":
                    led.value(1)  # Turn LED on
                    print("LED turned ON")
                elif message == "LED_OFF":
                    led.value(0)  # Turn LED off
                    print("LED turned OFF")
        except OSError as err:
            print("Error:", err)
            await asyncio.sleep(5)

# Main async function
async def main(e):
    await receive_messages(e)

# Run the async program
try:
    asyncio.run(main(e))
except KeyboardInterrupt:
    print("Stopping receiver...")
    led.value(0)  # Turn off LED on exit
    e.active(False)
    sta.active(False)
