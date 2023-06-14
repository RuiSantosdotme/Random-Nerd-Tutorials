# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-digital-inputs-python/

from gpiozero import Button, LED
from signal import pause

led = LED(14)
button = Button (4)

button.when_pressed = led.toggle

pause()