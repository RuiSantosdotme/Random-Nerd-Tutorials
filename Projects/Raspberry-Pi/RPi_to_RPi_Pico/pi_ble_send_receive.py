# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/bidirecional-ble-communication-raspberry-pi-and-pico/

import asyncio
from bleak import BleakClient, uuids
from gpiozero import LED

connected = False
led = LED(17)

# REPLACE WITH YOUR PICO MAC ADDRESS
pico_address = "FF:FF:FF:FF:FF:FF"

SERVICE_UUID = uuids.normalize_uuid_16(0x1848)
WRITE_CHARACTERISTIC_UUID = uuids.normalize_uuid_16(0x2A6E)
READ_CHARACTERISTIC_UUID = uuids.normalize_uuid_16(0x2A6F)

message_count = 0

async def send_data_task(client):
    global message_count
    while True:
        message_count += 1
        message = f"From central: {message_count}".encode("utf-8")
        
        try:
            if not client.is_connected:
                break
                
            await client.write_gatt_char(WRITE_CHARACTERISTIC_UUID, message)
            print(f"Central sent: {message.decode('utf-8')}")
        except Exception as e:
            print(f"Send error: {e}")
            break
        
        await asyncio.sleep(5)


async def receive_data_task(client):
    last_message = None
    while True:
        try:
            if not client.is_connected:
                break

            response = await client.read_gatt_char(READ_CHARACTERISTIC_UUID)

            if response:
                message = response.decode("utf-8")
                if message != last_message:
                    print(f"Central received: {message}")
                    last_message = message

        except Exception as e:
            print(f"Receive error: {e}")
            break
        
        await asyncio.sleep(2)


async def blink_task():
    global connected 
    print("blink task started")
    while True:
        led.toggle() 
        blink = 1000 if connected else 250
        await asyncio.sleep(blink / 1000)


async def connect_and_communicate(address):
    global connected
    print(f"Connecting to {address}...")

    blink_task_obj = asyncio.create_task(blink_task())

    while True:
        client = None
        try:
            client = BleakClient(address)
            await client.connect()
            connected = True
            print("Connected to Pico")

            tasks = [
                asyncio.create_task(send_data_task(client)),
                asyncio.create_task(receive_data_task(client))
            ]
            
            done, pending = await asyncio.wait(tasks, timeout=30, return_when=asyncio.FIRST_COMPLETED)
            
            for t in pending:
                t.cancel()

        except asyncio.CancelledError:
            raise
        except Exception as e:
            print(f"Connection failed or lost: {e}")
        finally:
            connected = False
            if client:
                try:
                    if client.is_connected:
                        await asyncio.wait_for(client.disconnect(), timeout=2.0)
                except:
                    pass
            print("Disconnected - trying to reconnect in 3 seconds...")
            await asyncio.sleep(3)


async def main():
    try:
        await connect_and_communicate(pico_address)
    except asyncio.CancelledError:
        print("\nCancelling tasks...")
    except KeyboardInterrupt:
        print("\nProgram stopped by user")
    except Exception as e:
        print(f"Unexpected error: {e}")
    finally:
        connected = False
        led.off()
        print("Cleanup completed.")


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        led.off()
