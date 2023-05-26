# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-pwm-python/

from gpiozero import PWMLED
from signal import pause

led = PWMLED(14)

#blink(on_time=1, off_time=1, fade_in_time=0, fade_out_time=0, n=None, background=True)
# fading an LED forever
led.blink(0, 0, 1, 1, None, True)
pause()
