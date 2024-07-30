# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-bh1750-micropython/

from machine import Pin, SoftI2C
from bh1750 import BH1750
import time

# Initialize I2C communication
i2c = SoftI2C(scl=Pin(5), sda=Pin(4), freq=400000)

# Create BH1750 object
light_sensor = BH1750(bus=i2c, addr=0x23)

try:
    # Read lux every 2 seconds
    while True:
        lux = light_sensor.luminance(BH1750.CONT_HIRES_1)
        print("Luminance: {:.2f} lux".format(lux))
        time.sleep(2)

except Exception as e:
    # Handle any exceptions during sensor reading
    print("An error occurred:", e)
