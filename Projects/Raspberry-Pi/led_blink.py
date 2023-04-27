# Complete Project Details: https://RandomNerdTutorials.com/projects-raspberry-pi/

from gpiozero import LED
from signal import pause

led = LED(14)

# blinking an LED forever
led.blink()
pause()
