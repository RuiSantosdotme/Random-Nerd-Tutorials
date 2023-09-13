# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ds18b20-micropython/

import machine, onewire, ds18x20, time

ds_pin = machine.Pin(22)
ds_sensor = ds18x20.DS18X20(onewire.OneWire(ds_pin))

roms = ds_sensor.scan()
number_devices = len(roms)
print('Number of found devices: ', number_devices)
print('Adresses: ', roms)

while True:
  ds_sensor.convert_temp()
  time.sleep_ms(750)
  for rom in roms:
    print(rom)
    
    tempC = ds_sensor.read_temp(rom)
    tempF = tempC * (9/5) +32
    
    print('temperature (ºC):', "{:.2f}".format(tempC))
    print('temperature (ºF):', "{:.2f}".format(tempF))
    print()
    
  time.sleep(5)
