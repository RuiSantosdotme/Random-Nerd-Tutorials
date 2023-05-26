# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-pwm-python/

from gpiozero import PWMOutputDevice
from time import sleep

led = PWMOutputDevice(14)

led.value = 1   #LED fully on
sleep(1)
led.value = 0.5  #LED half-brightness
sleep(1)
led.value = 0    #LED fully off
sleep(1)

try:
  # Fade in and out forever
  while True:
    #fade in
    for duty_cycle in range(0, 100, 1):
      led.value = duty_cycle/100.0
      sleep(0.05)

    #fade out
    for duty_cycle in range(100, 0, -1):
      led.value = duty_cycle/100.0
      sleep(0.05)

except KeyboardInterrupt:
  print("Stop the program and turning off the LED")
  led.value = 0
  pass