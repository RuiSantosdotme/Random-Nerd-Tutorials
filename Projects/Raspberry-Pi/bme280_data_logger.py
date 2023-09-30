# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-bme280-data-logger/

import smbus2
import bme280
import os
import time
import pytz

# BME280 sensor address (default address)
address = 0x76

# Initialize I2C bus
bus = smbus2.SMBus(1)

# Load calibration parameters
calibration_params = bme280.load_calibration_params(bus, address)

# create a variable to control the while loop
running = True

# Check if the file exists before opening it in 'a' mode (append mode)
file_exists = os.path.isfile('sensor_readings_bme280.txt')
file = open('sensor_readings_bme280.txt', 'a')

# Write the header to the file if the file does not exist
if not file_exists:
    file.write('Time and Date, temperature (ºC), temperature (ºF), humidity (%), pressure (hPa)\n')

# loop forever
while running:
    try:
        # Read sensor data
        data = bme280.sample(bus, address, calibration_params)

        # Extract temperature, pressure, humidity, and corresponding timestamp
        temperature_celsius = data.temperature
        humidity = data.humidity
        pressure = data.pressure
        timestamp = data.timestamp

        # Adjust timezone
        # Define the timezone you want to use (list of timezones: https://gist.github.com/mjrulesamrat/0c1f7de951d3c508fb3a20b4b0b33a98)
        desired_timezone = pytz.timezone('Europe/Lisbon')  # Replace with your desired timezone

        # Convert the datetime to the desired timezone
        timestamp_tz = timestamp.replace(tzinfo=pytz.utc).astimezone(desired_timezone)

        # Convert temperature to Fahrenheit
        temperature_fahrenheit = (temperature_celsius * 9/5) + 32

        # Print the readings
        print(timestamp_tz.strftime('%H:%M:%S %d/%m/%Y') + " Temp={0:0.1f}ºC, Temp={1:0.1f}ºF, Humidity={2:0.1f}%, Pressure={3:0.2f}hPa".format(temperature_celsius, temperature_fahrenheit, humidity, pressure))

        # Save time, date, temperature, humidity, and pressure in .txt file
        file.write(timestamp_tz.strftime('%H:%M:%S %d/%m/%Y') + ', {:.2f}, {:.2f}, {:.2f}, {:.2f}\n'.format(temperature_celsius, temperature_fahrenheit, humidity, pressure))

        time.sleep(10)

    except KeyboardInterrupt:
        print('Program stopped')
        running = False
        file.close()
    except Exception as e:
        print('An unexpected error occurred:', str(e))
        running = False
        file.close()
