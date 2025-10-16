# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/ble-raspberry-pi-and-pi-pico-w/

import asyncio
from bleak import BleakClient, uuids
from gpiozero import LED 

connected = False   

led = LED(17)

# Replace with the MAC address of your Pico 
pico_address = "FF:FF:FF:FF:FF:FF"

# Service UUID (0x1848)
SERVICE_UUID = uuids.normalize_uuid_16(0x1848)
WRITE_CHARACTERISTIC_UUID = uuids.normalize_uuid_16(0x2A6E) # Central writes here
READ_CHARACTERISTIC_UUID = uuids.normalize_uuid_16(0x2A6F)  # Central reads here

async def receive_data_task(client):
    """Receive data from the peripheral device."""
    while True:
        try:
            response = await client.read_gatt_char(READ_CHARACTERISTIC_UUID)
            print(f"Central received: {response.decode('utf-8')}")
            await asyncio.sleep(1)
        except Exception as e:
            print(f"Error receiving data: {e}")
            break

async def blink_task():
    global connected 
    print("blink task started")
    while True:
        led.toggle() 
        blink = 1000  if connected else 250
        await asyncio.sleep(blink / 1000)

async def connect_and_communicate(address):
    global connected
    """Connect to the peripheral and manage data exchange."""
    print(f"Connecting to {address}...")

    async with BleakClient(address) as client:
        connected = client.is_connected
        print(f"Connected: {connected}")

        # Create tasks for sending and receiving data
        tasks = [
            asyncio.create_task(receive_data_task(client)),
            asyncio.create_task(blink_task())
        ]
        await asyncio.gather(*tasks)
    connected = False

# Run the connection and communication
loop = asyncio.get_event_loop()
loop.run_until_complete(connect_and_communicate(pico_address))