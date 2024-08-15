# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp32-neo-6m-gps/

import machine
from time import sleep

# Define the UART pins and create a UART object
gps_serial = machine.UART(2, baudrate=9600, tx=17, rx=16)

while True:
    if gps_serial.any():
        line = gps_serial.readline()  # Read a complete line from the UART
        if line:
            line = line.decode('utf-8')
            print(line.strip())
    sleep(0.5)
