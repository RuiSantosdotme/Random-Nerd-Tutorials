# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/ble-raspberry-pi-and-pi-pico-w/

import asyncio
import aioble
import bluetooth
from machine import Pin

# Bluetooth configuration
_SERVICE_UUID = bluetooth.UUID(0x1848)
_WRITE_CHARACTERISTIC_UUID = bluetooth.UUID(0x2A6E) # Central writes here
_READ_CHARACTERISTIC_UUID = bluetooth.UUID(0x2A6F)  # Peripheral writes message here

BLE_NAME = "Pico W Peripheral"

# Initialize LED
led = Pin("LED", Pin.OUT)

connected = False

# Register GATT server
ble_service = aioble.Service(_SERVICE_UUID)
read_characteristic = aioble.Characteristic(
    ble_service, _READ_CHARACTERISTIC_UUID, read=True, notify=True
)
aioble.register_services(ble_service)

# Helper to encode the message
def _encode_message(message):
    return message.encode('utf-8')

# Task to handle LED blinking and message sending
async def send_task():
    global connected
    message_count = 1
    while True:
        led.toggle()
        blink = 1000 if connected else 250
        if connected:
            message = f"Hello from {BLE_NAME}! Count: {message_count}"
            read_characteristic.write(_encode_message(message), send_update=True)
            message_count += 1
            print(f"Sent: {message}")
        await asyncio.sleep_ms(blink)

# Serially wait for connections
async def peripheral_task():
    global connected
    # Show MAC address once at start
    ble = bluetooth.BLE()
    _, mac_address = ble.config('mac')
    formatted_mac = ':'.join('{:02X}'.format(b) for b in mac_address)
    print(f"Bluetooth MAC Address: {formatted_mac}")
    
    while True:
        try:
            async with await aioble.advertise(
                2000, name=BLE_NAME, services=[_SERVICE_UUID], appearance=768
            ) as connection:
                connected = True
                print("Connection from", connection.device)
                await connection.disconnected()
        except Exception as e:
            print("Error in peripheral_task:", e)
        finally:
            connected = False
            print(f"{BLE_NAME} disconnected")
            await asyncio.sleep_ms(100)

# Run both tasks
async def main():
    t1 = asyncio.create_task(send_task())
    t2 = asyncio.create_task(peripheral_task())
    await asyncio.gather(t1, t2)

# Run the program
asyncio.run(main())