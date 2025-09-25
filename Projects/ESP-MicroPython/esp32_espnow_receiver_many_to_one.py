# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp-now-esp32-many-to-one/

import network
import aioespnow
import asyncio
import ujson
from machine import Pin, I2C
import ssd1306

# Initialize I2C for OLED
try:
    i2c = I2C(0, scl=Pin(22), sda=Pin(21))  # Adjust pins as needed
    display = ssd1306.SSD1306_I2C(128, 64, i2c, addr=0x3C)
    print("SSD1306 initialized")
except Exception as err:
    print("Failed to initialize SSD1306:", err)
    raise

# Initialize Wi-Fi in station mode
sta = network.WLAN(network.STA_IF)
try:
    sta.active(True)
    sta.config(channel=1)  # Set channel explicitly
    sta.disconnect()
except OSError as err:
    print("Failed to initialize Wi-Fi:", err)
    raise

# Initialize AIOESPNow
e = aioespnow.AIOESPNow()
try:
    e.active(True)
except OSError as err:
    print("Failed to initialize AIOESPNow:", err)
    raise

# Sender's MAC addresses (replace with actual sender MACs)
sender_mac_1 = b'\x24\x0a\xc4\x31\x40\x50'  # First sender's MAC (Board ID=1)
sender_mac_2 = b'\x30\xae\xa4\xf6\x7d\x4c'  # Second sender's MAC (Board ID=2)

# Add peers
try:
    e.add_peer(sender_mac_1)
except OSError as err:
    print(f"Failed to add peer {sender_mac_1.hex()}:", err)
    raise

try:
    e.add_peer(sender_mac_2)
except OSError as err:
    print(f"Failed to add peer {sender_mac_2.hex()}:", err)
    raise

# Dictionary to store latest readings for each board
board_readings = {
    1: {'temp': 0.0, 'hum': 0.0, 'readingId': 0},
    2: {'temp': 0.0, 'hum': 0.0, 'readingId': 0}
}

# Update OLED display with temperature and humidity for both boards on separate lines.
def update_display():
    try:
        display.fill(0)
        # Board 1 data
        display.text("Board 1:", 0, 0)
        display.text(f"Temp: {board_readings[1]['temp']:.1f} C", 0, 10)
        display.text(f"Hum: {board_readings[1]['hum']:.1f} %", 0, 20)
        # Board 2 data
        display.text("Board 2:", 0, 32)
        display.text(f"Temp: {board_readings[2]['temp']:.1f} C", 0, 42)
        display.text(f"Hum: {board_readings[2]['hum']:.1f} %", 0, 52)
        display.show()
        print("Display updated")
    except Exception as err:
        print("Error updating display:", err)

# Async function to receive and process messages.
async def receive_messages(e):
    print("Listening for ESP-NOW messages...")
    while True:
        try:
            async for mac, msg in e:
                try:
                    # Decode and parse JSON message
                    json_str = msg.decode('utf-8')
                    data = ujson.loads(json_str)
                    
                    # Extract parameters
                    board_id = data['id']
                    temperature = data['temp']
                    humidity = data['hum']
                    reading_id = data['readingId']
                    
                    # Store in board_readings dictionary
                    if board_id in (1, 2):
                        board_readings[board_id] = {
                            'temp': temperature,
                            'hum': humidity,
                            'readingId': reading_id
                        }
                        # Update OLED display
                        update_display()
                    
                    # Display on MicroPython terminal
                    print(f"\nReceived from {mac.hex()}:")
                    print(f"  Board ID: {board_id}")
                    print(f"  Temperature: {temperature} C")
                    print(f"  Humidity: {humidity} %")
                    print(f"  Reading ID: {reading_id}")
                except (ValueError, KeyError) as err:
                    print(f"Error parsing JSON: {err}")
        except OSError as err:
            print("Receive error:", err)
            await asyncio.sleep(5)

async def main(e):
    await receive_messages(e)

# Run the async program
try:
    asyncio.run(main(e))
except KeyboardInterrupt:
    print("Stopping receiver...")
    e.active(False)
    sta.active(False)
