# Complete Project Details: https://RandomNerdTutorials.com/projects-raspberry-pi/

from gpiozero import LED
from time import sleep

led = LED(14)

# blinking an LED forever
while True:
  #set the led ON for one second
  led.on()
  sleep(1)
  #set the led ON for one second
  led.off()
  sleep(1)
