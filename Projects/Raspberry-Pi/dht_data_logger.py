# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-temperature-humidity-data-logger/

import adafruit_dht
import board
import time
import os

# Sensor data pin is connected to GPIO 4
sensor = adafruit_dht.DHT22(board.D4)
# Uncomment for DHT11
#sensor = adafruit_dht.DHT11(board.D4)

# create a variable to control the while loop
running = True

# Check if the file exists before opening it in 'a' mode (append mode)
file_exists = os.path.isfile('sensor_readings.txt')
file = open('sensor_readings.txt', 'a')

# Write the header to the file if the file does not exist
if not file_exists:
    file.write('Time and Date, temperature (ºC), temperature (ºF), humidity (%)\n')

# loop forever
while running:
    try:
        # read the humidity and temperature
        temperature_c = sensor.temperature
        temperature_f = temperature_c * (9 / 5) + 32
        humidity = sensor.humidity

        #print readings on the shell
        print("Temp={0:0.1f}ºC, Temp={1:0.1f}ºF, Humidity={2:0.1f}%".format(temperature_c, temperature_f, humidity))

        # save time, date, temperature, and humidity in .txt file
        file.write(time.strftime('%H:%M:%S %d/%m/%Y') + ', {:.2f}, {:.2f}, {:.2f}\n'.format(temperature_c, temperature_f, humidity))

        # log new readings every 10 seconds
        time.sleep(10)

    except RuntimeError as error:
        # Errors happen fairly often, DHT's are hard to read, just keep going
        print(error.args[0])
        time.sleep(2.0)
        continue

    except KeyboardInterrupt:
        print('Program stopped')
        running = False
        file.close()

    except Exception as e:
        print('An unexpected error occurred:', str(e))
        running = False
        file.close()
        sensor.exit()
