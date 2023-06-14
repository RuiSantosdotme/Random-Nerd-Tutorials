# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-digital-inputs-python/

from gpiozero import Button, LED

led = LED(14)
button = Button(4)

while True:
    if button.is_pressed:
        led.on()
    else:
        led.off()