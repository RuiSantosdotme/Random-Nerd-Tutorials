# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-control-outputs-based-on-time/

from gpiozero import LED, TimeOfDay
from datetime import time
from signal import pause

led1 = LED(17)
led2 = LED(27)

tod1 = TimeOfDay(time(16,30), time(16,45), utc=False)
tod2 = TimeOfDay(time(16,45), time(16,50), utc=False)

while True:
    if tod1.is_active:
        led1.on()
    else:
        led1.off()
    
    if tod2.is_active:
        led2.on()
    else: 
        led2.off()
