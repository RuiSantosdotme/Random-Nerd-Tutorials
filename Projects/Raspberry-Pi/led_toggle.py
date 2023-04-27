# Complete Project Details: https://RandomNerdTutorials.com/projects-raspberry-pi/

from gpiozero import LED
from time import sleep

led = LED(14)

# blinking an LED forever
while True:
  led.toggle()
  sleep(1)
