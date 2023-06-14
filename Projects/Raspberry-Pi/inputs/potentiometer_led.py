# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-analog-inputs-python-mcp3008/

from gpiozero import PWMLED, MCP3008
from time import sleep

#create an object called pot that refers to MCP3008 channel 0
pot = MCP3008(0)

#create a PWMLED object called led that refers to GPIO 14
led = PWMLED(14)

while True:
    if(pot.value < 0.001):
        led.value = 0
    else:
        led.value = pot.value
    print(pot.value)
    sleep(0.1)