# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp32-esp-now-two-way/

import network
import aioespnow
import asyncio
import time
import ujson
from machine import Pin, I2C
import BME280
import ssd1306

# Initialize I2C for BME280 and SSD1306
i2c = I2C(0, scl=Pin(22), sda=Pin(21))

# Initialize BME280 sensor
try:
    bme = BME280.BME280(i2c=i2c, address=0x76)
    print("BME280 initialized")
except Exception as err:
    print("Failed to initialize BME280:", err)
    raise

# Initialize SSD1306 OLED display
try:
    display = ssd1306.SSD1306_I2C(128, 64, i2c, addr=0x3C)
    print("SSD1306 initialized")
except Exception as err:
    print("Failed to initialize SSD1306:", err)
    raise

# Initialize Wi-Fi in station mode
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.config(channel=1) # set Wi-Fi channel for more stable communication
sta.disconnect()
print("Wi-Fi initialized")

# Initialize AIOESPNow
e = aioespnow.AIOESPNow()
try:
    e.active(True)
    print("AIOESPNow initialized")
except OSError as err:
    print("Failed to initialize AIOESPNow:", err)
    raise

# Receiver MAC address (the board you want to send data to)
peer_mac = b'\xff\xff\xff\xff\xff\xff'

# Add peer
try:
    e.add_peer(peer_mac)
except OSError as err:
    print("Failed to add peer:", err)
    raise

# Variables to store readings and status
last_send_status = " "
incoming_readings = {'temp': 0.0, 'hum': 0.0, 'pres': 0.0}

# Function to get BME280 readings
def get_readings():
    try:
        temp = float(bme.temperature[:-1]) # Remove 'C'
        hum = float(bme.humidity[:-1])     # Remove '%'
        pres = float(bme.pressure[:-3])    # Remove 'hPa'
        print("BME280 readings:", temp, hum, pres)
        return temp, hum, pres
    except Exception as err:
        print("Error reading BME280:", err)
        return 0.0, 0.0, 0.0

# Function to update OLED display
def update_display():
    try:
        display.fill(0)
        display.text("INCOM. READINGS", 0, 0)
        display.text("Temp: {:.1f} C".format(incoming_readings['temp']), 0, 15)
        display.text("Hum: {:.1f} %".format(incoming_readings['hum']), 0, 25)
        display.text("Pres: {:.1f} hPa".format(incoming_readings['pres']), 0, 35)
        display.text(last_send_status, 0, 55)
        display.show()
        print("Display updated")
    except Exception as err:
        print("Error updating display:", err)

# Async function to send messages
async def send_messages(e, peer):
    global last_send_status
    while True:
        try:
            print("Sending data")
            temp, hum, pres = get_readings()
            # Create JSON string
            data_dict = {"temp": temp, "hum": hum, "pres": pres}
            json_str = ujson.dumps(data_dict)
            data = json_str.encode('utf-8')  # Convert to bytes
            print("Sending JSON:", json_str)
            if await e.asend(peer, data, sync=True):
                print("Sent with success")
                last_send_status = "Delivery Success :)"
            else:
                print("Send failed")
                last_send_status = "Delivery Fail :("
            update_display()
            print("Sending task complete")
            await asyncio.sleep(10)  # Send every 10 seconds
        except OSError as err:
            print("Send error:", err)
            last_send_status = "Delivery Fail :("
            update_display()
            await asyncio.sleep(0.1)  # Shorter delay in case of error

# Async function to receive messages
async def receive_messages(e):
    global incoming_readings
    while True:
        try:
            print("Checking for messages")
            async for mac, msg in e:
                try:
                    # Decode bytes to string and parse JSON
                    json_str = msg.decode('utf-8')
                    data_dict = ujson.loads(json_str)
                    temp = data_dict['temp']
                    hum = data_dict['hum']
                    pres = data_dict['pres']
                    incoming_readings['temp'] = temp
                    incoming_readings['hum'] = hum
                    incoming_readings['pres'] = pres
                    print("\nINCOMING READINGS")
                    print("Temperature: {:.1f} ºC".format(temp))
                    print("Humidity: {:.1f} %".format(hum))
                    print("Pressure: {:.1f} hPa".format(pres))
                    update_display()
                except (ValueError, KeyError) as err:
                    print("Error parsing JSON:", err)
            await asyncio.sleep(0.01)  # Yield if no received messages
        except OSError as err:
            print("Receive error:", err)
            await asyncio.sleep(0.1)  # Shorter delay in case of error

# Main async function
async def main(e, peer):
    print("Starting main loop")
    await asyncio.gather(send_messages(e, peer), receive_messages(e))

# Run the async program
try:
    print("Starting transceiver...")
    asyncio.run(main(e, peer_mac))
except KeyboardInterrupt:
    print("Stopping transceiver...")
    e.active(False)
    sta.active(False)
except Exception as err:
    print("Main loop error:", err)
finally:
    print("Cleaning up...")
    e.active(False)
    sta.active(False)
