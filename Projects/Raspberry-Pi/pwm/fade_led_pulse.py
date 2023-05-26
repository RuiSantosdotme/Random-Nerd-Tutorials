# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-pwm-python/

from gpiozero import PWMLED
from signal import pause

led = PWMLED(14)

# pulse an LED forever
led.pulse()
pause()