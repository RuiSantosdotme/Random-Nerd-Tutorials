# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-w-bluetooth-low-energy-micropython/

from micropython import const
import uasyncio as asyncio
import aioble
import bluetooth
import struct

# org.bluetooth.service.environmental_sensing
_ENV_SENSE_UUID = bluetooth.UUID(0x181A)
# org.bluetooth.characteristic.temperature
_ENV_SENSE_TEMP_UUID = bluetooth.UUID(0x2A6E)

# Name of the peripheral you want to connect
peripheral_name="RPi-Pico"

# Helper to decode the temperature characteristic encoding (sint16, hundredths of a degree).
def _decode_temperature(data):
    try:
        if data is not None:
            return struct.unpack("<h", data)[0] / 100
    except Exception as e:
        print("Error decoding temperature:", e)
    return None

async def find_temp_sensor():
    # Scan for 5 seconds, in active mode, with a very low interval/window (to
    # maximize detection rate).
    async with aioble.scan(5000, interval_us=30000, window_us=30000, active=True) as scanner:
        async for result in scanner:
            print(result.name())
            # See if it matches our name and the environmental sensing service.
            if result.name() == peripheral_name and _ENV_SENSE_UUID in result.services():
                return result.device
    return None

async def main():
    while True:
        device = await find_temp_sensor()
        if not device:
            print("Temperature sensor not found. Retrying...")
            await asyncio.sleep_ms(5000)  # Wait for 5 seconds before retrying
            continue

        try:
            print("Connecting to", device)
            connection = await device.connect()
        except asyncio.TimeoutError:
            print("Timeout during connection. Retrying...")
            await asyncio.sleep_ms(5000)  # Wait for 5 seconds before retrying
            continue

        async with connection:
            try:
                temp_service = await connection.service(_ENV_SENSE_UUID)
                temp_characteristic = await temp_service.characteristic(_ENV_SENSE_TEMP_UUID)
            except asyncio.TimeoutError:
                print("Timeout discovering services/characteristics. Retrying...")
                await asyncio.sleep_ms(5000)  # Wait for 5 seconds before retrying
                continue

            while True:
                try:
                    temp_data = await temp_characteristic.read()
                    if temp_data is not None:
                        temp_deg_c = _decode_temperature(temp_data)
                        if temp_deg_c is not None:
                            print("Temperature: {:.2f}".format(temp_deg_c))
                        else:
                            print("Invalid temperature data")
                    else:
                        print("Error reading temperature: None")
                except Exception as e:
                    print("Error in main loop:", e)
                    break  # Break out of the inner loop and attempt to reconnect

                await asyncio.sleep_ms(1000)

# Create an Event Loop
loop = asyncio.get_event_loop()
# Create a task to run the main function
loop.create_task(main())

try:
    # Run the event loop indefinitely
    loop.run_forever()
except Exception as e:
    print('Error occurred: ', e)
except KeyboardInterrupt:
    print('Program Interrupted by the user')
