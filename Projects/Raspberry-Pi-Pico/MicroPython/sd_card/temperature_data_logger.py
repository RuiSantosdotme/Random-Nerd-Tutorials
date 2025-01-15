# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-microsd-card-micropython/

from time import sleep
from machine import Timer, SPI, Pin
from picozero import pico_temp_sensor
import sdcard, os

# Constants
SPI_BUS = 0
SCK_PIN = 2
MOSI_PIN = 3
MISO_PIN = 4
CS_PIN = 5
SD_MOUNT_PATH = '/sd'
FILE_PATH = '/sd/temperature.txt'

# Init SPI communication
spi = SPI(SPI_BUS, sck=Pin(SCK_PIN), mosi=Pin(MOSI_PIN), miso=Pin(MISO_PIN))
cs = Pin(CS_PIN)

def mount_sdcard(spi, cs_pin):
    try:
        sd = sdcard.SDCard(spi, cs_pin)
        # Mount microSD card
        os.mount(sd, SD_MOUNT_PATH)
        # List files on the microSD card
        print(os.listdir(SD_MOUNT_PATH))
    
    except Exception as e:
        print('An error occurred mounting the SD card:', e)

def log_temperature(timer):
    try:
        # Read the internal temperature sensor value
        temperature = pico_temp_sensor.temp
        # Format temperature
        temperature_string = "{:.2f} °C\n".format(temperature)
        
        # Write to the file
        with open(FILE_PATH, 'a') as file:
            file.write(temperature_string)
            print("Temperature logged successfully.")
    except Exception as e:
        print('An error occurred accessing the file or getting temperature', e)

# Mount SD card
mount_sdcard(spi, cs)

# Log temperature when the program first runs
log_temperature(None)

# Create a timer that calls log_temperature every 30 seconds
log_timer = Timer(period=30000, mode=Timer.PERIODIC, callback=log_temperature)

# Keep the program running
try:
    while True:
        sleep(0.1)
except KeyboardInterrupt:
    # Clean up and stop the timer on keyboard interrupt
    log_timer.deinit()
    print("Keyboard Interrupt")