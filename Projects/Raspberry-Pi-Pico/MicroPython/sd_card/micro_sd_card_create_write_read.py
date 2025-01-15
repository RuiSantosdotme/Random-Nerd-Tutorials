# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-microsd-card-micropython/
 
from machine import SPI, Pin
import sdcard, os

# Constants
SPI_BUS = 0
SCK_PIN = 2
MOSI_PIN = 3
MISO_PIN = 4
CS_PIN = 5
SD_MOUNT_PATH = '/sd'
FILE_PATH = 'sd/sd_file.txt'

try:
    # Init SPI communication
    spi = SPI(SPI_BUS,sck=Pin(SCK_PIN), mosi=Pin(MOSI_PIN), miso=Pin(MISO_PIN))
    cs = Pin(CS_PIN)
    sd = sdcard.SDCard(spi, cs)
    # Mount microSD card
    os.mount(sd, SD_MOUNT_PATH)
    # List files on the microSD card
    print(os.listdir(SD_MOUNT_PATH))
    
    # Create new file on the microSD card
    with open(FILE_PATH, "w") as file:
        # Write to the file
        file.write("Testing microSD Card \n")
        
    # Check that the file was created:
    print(os.listdir(SD_MOUNT_PATH))
    
    # Open the file in reading mode
    with open(FILE_PATH, "r") as file:
        # read the file content
        content = file.read()
        print("File content:", content)  
    
except Exception as e:
    print('An error occurred:', e)