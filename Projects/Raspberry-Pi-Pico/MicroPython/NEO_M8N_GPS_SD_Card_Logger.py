# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-neo-m8n-gps-micropytho

from machine import UART, Pin, Timer, SPI
from time import sleep, ticks_ms, ticks_diff
from micropyGPS import MicropyGPS
import sdcard, os

# microSD card pin definition
SPI_BUS = 0
SCK_PIN = 18
MOSI_PIN = 19
MISO_PIN = 16
CS_PIN = 17

# microSD card filesystem mount path
SD_MOUNT_PATH = '/sd'
# file path to save data
FILE_PATH = '/sd/data.txt'

# Init SPI communication for the microSD card
spi = SPI(SPI_BUS, sck=Pin(SCK_PIN), mosi=Pin(MOSI_PIN), miso=Pin(MISO_PIN))
cs = Pin(CS_PIN)

# Instantiate the micropyGPS object
my_gps = MicropyGPS()

# Define the UART pins and create a UART object
gps_serial = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5))

def mount_sdcard(spi, cs_pin):
    try:
        sd = sdcard.SDCard(spi, cs_pin)
        # Mount microSD card
        os.mount(sd, SD_MOUNT_PATH)
        # List files on the microSD card
        print(os.listdir(SD_MOUNT_PATH))
    
    except Exception as e:
        print('An error occurred mounting the SD card:', e)
        
def write_to_sd(data, file_path):
    try:
        # Open the file in append mode ('a')
        with open(file_path, 'a') as file:
            file.write(data + '\n')
        print("Data logged successfully.")
    except Exception as e:
        print(f"An error occurred while writing to SD card: {e}")

# Convert Longitude and Latitude to decimal degrees
def convert_to_decimal_degrees(coord):
    degrees = coord[0]
    minutes = coord[1]
    hemisphere = coord[2]
    
    decimal_degrees = degrees + (minutes / 60)
    
    # Make the value negative if it's in the western or southern hemisphere
    if hemisphere in ['S', 'W']:
        decimal_degrees *= -1
    
    return decimal_degrees

# Mount SD card
mount_sdcard(spi, cs)

# Initialize a timer variable
last_print_time = ticks_ms()  # Get the current time in milliseconds

while True:
    try:
        # Check if data is available in the UART buffer
        while gps_serial.any():
            data = gps_serial.read()
            for byte in data:
                stat = my_gps.update(chr(byte))
                if stat is not None:
                    # Check if 3 seconds have passed since the last print
                    current_time = ticks_ms()
                    if ticks_diff(current_time, last_print_time) >= 3000:
                        # Get longitude, latitude, and altitude
                        longitude = convert_to_decimal_degrees(my_gps.longitude)
                        latitude = convert_to_decimal_degrees(my_gps.latitude)
                        altitude = my_gps.altitude
                        GPS_data = f"{longitude},{latitude},{altitude}"
                        
                        # Print the GPS data
                        print(GPS_data)
                        print()
                        
                        # Save data on the microSD card
                        write_to_sd(GPS_data, FILE_PATH);
                        
                        # Update the last print time
                        last_print_time = current_time
            
    except Exception as e:
        print(f"An error occurred: {e}")