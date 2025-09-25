# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp-now-esp32-many-to-one/

import network
import aioespnow
import asyncio
import time
import ujson
from machine import Pin, I2C
import BME280

# Board ID
BOARD_ID = 1

# Receiver's MAC address
peer_mac = b'\xff\xff\xff\xff\xff\xff'

# Interval for sending data (in seconds)
send_interval = 10 

# Initialize I2C and BME280
try:
    i2c = I2C(0, scl=Pin(22), sda=Pin(21))  # Adjust pins as needed
    bme = BME280.BME280(i2c=i2c, address=0x76)
except OSError as err:
    print("Failed to initialize BME280:", err)
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

# Add peer
try:
    e.add_peer(peer_mac)
except OSError as err:
    print("Failed to add peer:", err)
    raise

# Counter for reading ID
reading_id = 0

def read_temperature():
    try:
        return float(bme.temperature[:-1])  # Remove 'C' from string
    except Exception as err:
        print("Error reading temperature:", err)
        return 0.0

def read_humidity():
    try:
        return float(bme.humidity[:-1])  # Remove '%' from string
    except Exception as err:
        print("Error reading humidity:", err)
        return 0.0

def prepare_sensor_data():
    global reading_id
    data = {
        'id': BOARD_ID,
        'temp': read_temperature(),
        'hum': read_humidity(),
        'readingId': reading_id
    }
    reading_id += 1
    # Serialize to JSON and encode to bytes
    return ujson.dumps(data).encode('utf-8')

async def send_messages(e, peer):
    while True:
        try:
            # Prepare and serialize sensor data
            message = prepare_sensor_data()
            # Send JSON bytes
            if await e.asend(peer, message, sync=True):
                print(f"Sent data: {message.decode('utf-8')}")
            else:
                print("Failed to send data")
        except OSError as err:
            print("Send error:", err)
            await asyncio.sleep(5)
        await asyncio.sleep(send_interval)  # Wait before next send

async def main(e, peer):
    try:
        await send_messages(e, peer)
    except Exception as err:
        print(f"Error in main: {err}")
        await asyncio.sleep(5)
        raise

# Run the async program
try:
    asyncio.run(main(e, peer_mac))
except KeyboardInterrupt:
    print("Stopping sender...")
    e.active(False)
    sta.active(False)
