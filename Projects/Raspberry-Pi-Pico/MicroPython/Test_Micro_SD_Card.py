# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-neo-m8n-gps-micropytho

from machine import SPI, Pin
import sdcard, os

# Constants
SPI_BUS = 0
SCK_PIN = 18
MOSI_PIN = 19
MISO_PIN = 16
CS_PIN = 17
SD_MOUNT_PATH = '/sd'

try:
    # Init SPI communication
    spi = SPI(SPI_BUS,sck=Pin(SCK_PIN), mosi=Pin(MOSI_PIN), miso=Pin(MISO_PIN))
    cs = Pin(CS_PIN)
    sd = sdcard.SDCard(spi, cs)
    # Mount microSD card
    os.mount(sd, SD_MOUNT_PATH)
    # List files on the microSD card
    print(os.listdir(SD_MOUNT_PATH))
    
except Exception as e:
    print('An error occurred:', e)