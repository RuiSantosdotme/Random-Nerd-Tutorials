# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp-now-esp32-one-to-many/

import network
import aioespnow
import asyncio
from machine import Pin

# Initialize Wi-Fi in station mode
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()

# Initialize AIOESPNow
e = aioespnow.AIOESPNow()
try:
    e.active(True)
except OSError as err:
    print("Failed to initialize AIOESPNow:", err)
    raise

# Define pins for pushbuttons
button1_pin = Pin(21, Pin.IN, Pin.PULL_UP)  # Button for Board 1
button2_pin = Pin(22, Pin.IN, Pin.PULL_UP)  # Button for Board 2
button3_pin = Pin(23, Pin.IN, Pin.PULL_UP)  # Button for Board 3

# Define MAC addresses for receiving boards
board1_mac = b'\xff\xff\xff\xff\xff\xff'  # MAC address for Board 1
board2_mac = b'\xff\xff\xff\xff\xff\xff'  # MAC address for Board 2
board3_mac = b'\xff\xff\xff\xff\xff\xff'  # MAC address for Board 3

# Build dictionary with pin and MAC variables
boards = {
    1: {"pin": button1_pin, "mac": board1_mac},
    2: {"pin": button2_pin, "mac": board2_mac},
    3: {"pin": button3_pin, "mac": board3_mac}
}

# Add peers
for board_num, info in boards.items():
    try:
        e.add_peer(info["mac"])
        print(f"Added Board {board_num} peer: {info['mac']}")
    except OSError as err:
        print(f"Failed to add Board {board_num} peer: {err}")
        raise

# Async function to monitor buttons and send toggle commands
async def monitor_buttons(e):
    # Track LED states (True = ON, False = OFF)
    led_states = {1: False, 2: False, 3: False}
    
    while True:
        for board_num, info in boards.items():
            button = info["pin"]
            if button.value() == 0:  # Button pressed (active low)
                led_states[board_num] = not led_states[board_num]  # Toggle LED
                command = "LED_ON" if led_states[board_num] else "LED_OFF"
                if await e.asend(info["mac"], command, sync=True):
                    print(f"Board {board_num}: {command}")
                else:
                    print(f"Board {board_num}: Failed to send")
                await asyncio.sleep(0.3)  # Debounce delay
        
        await asyncio.sleep(0.1)  # Short loop delay

# Main async function
async def main(e):
    await monitor_buttons(e)

# Run the async program
try:
    asyncio.run(main(e))
except KeyboardInterrupt:
    print("Stopping...")
    e.active(False)
    sta.active(False)
