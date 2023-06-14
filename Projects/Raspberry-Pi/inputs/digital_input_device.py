# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-digital-inputs-python/

from gpiozero import DigitalInputDevice, LED

led = LED(14)
input = DigitalInputDevice(4)

while True:
    if input.value:
        led.on()
    else:
        led.off()