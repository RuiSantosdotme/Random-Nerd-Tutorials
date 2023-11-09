# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-control-outputs-based-on-time/

from gpiozero import LED, TimeOfDay
from datetime import time
from signal import pause

led1 = LED(17)
led2 = LED(27)

tod1 = TimeOfDay(time(16,30), time(16,45), utc=False)
tod2 = TimeOfDay(time(16,45), time(16,50), utc=False)

tod1.when_activated = led1.on
tod1.when_deactivated = led1.off

tod2.when_activated = led2.on
tod2.when_deactivated = led2.off

pause()
