# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/bidirecional-ble-communication-raspberry-pi-and-pico/

import asyncio
import aioble
import bluetooth

# Bluetooth configuration
_SERVICE_UUID = bluetooth.UUID(0x1848)
# Characteristic where the Raspberry Pi writes data TO the Pico (Inbox)
_PI_TO_PICO_CHARACTERISTIC_UUID = bluetooth.UUID(0x2A6E)
# Characteristic where the Pico writes data TO the Raspberry Pi (Outbox)
_PICO_TO_PI_CHARACTERISTIC_UUID = bluetooth.UUID(0x2A6F)
BLE_NAME = "Pico W Peripheral"

# Register GATT server
ble_service = aioble.Service(_SERVICE_UUID)

# Inbox: Raspberry Pi writes data to the Pico
rx_characteristic = aioble.Characteristic(
    ble_service, 
    _PI_TO_PICO_CHARACTERISTIC_UUID, 
    read=True, 
    write=True, 
    capture=True
)

# Outbox: Pico writes data to the Raspberry Pi
tx_characteristic = aioble.Characteristic(
    ble_service, 
    _PICO_TO_PI_CHARACTERISTIC_UUID, 
    read=True, 
    write=True, 
    notify=True, 
    capture=True
)

aioble.register_services(ble_service)

# Helper functions
def _encode_message(message):
    """Encode a message to bytes."""
    return message.encode('utf-8')

def _decode_message(data):
    """Decode a message from bytes."""
    return data.decode('utf-8')

# Task to handle sending data
async def send_task():
    message_count = 1
    while True:
        message = f" From {BLE_NAME}!: {message_count}"
        message_count += 1
        try:
            tx_characteristic.write(_encode_message(message), send_update=True)
            print(f"Sent: {message}")
        except Exception as e:
            print(f"Error while sending data: {e}")
       
        await asyncio.sleep_ms(1000)

# Task to handle receiving data
async def receive_task():
    last_data = None                     
    while True:
        try:
            data = rx_characteristic.read()
            
            if data and data != last_data:          
                decoded = _decode_message(data)
                print(f"Received: {decoded}")
                last_data = data                    
                
            await asyncio.sleep_ms(500)
            
        except Exception as e:
            print(f"Error receiving data: {e}")
            await asyncio.sleep_ms(500)

# Serially wait for connections
async def peripheral_task():
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
                print("Connection from", connection.device)
                await connection.disconnected()
               
        except Exception as e:
            print("Error in peripheral_task:", e)
        finally:
            print(f"{BLE_NAME} disconnected")
            await asyncio.sleep_ms(100)

# Run all tasks
async def main():
    t1 = asyncio.create_task(send_task())
    t2 = asyncio.create_task(receive_task())
    t3 = asyncio.create_task(peripheral_task())
    await asyncio.gather(t1, t2, t3)

# Run the program
asyncio.run(main())
