# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-dc-motor-micropython/

from dcmotor import DCMotor
from machine import Pin, PWM
from time import sleep

frequency = 1000

pin1 = Pin(3, Pin.OUT)
pin2 = Pin(4, Pin.OUT)
enable = PWM(Pin(2), frequency)

dc_motor = DCMotor(pin1, pin2, enable)

# Set min duty cycle (15000) and max duty cycle (65535) 
#dc_motor = DCMotor(pin1, pin2, enable, 15000, 65535)

try:
    print('Forward with speed: 50%')
    dc_motor.forward(50)
    sleep(5)
    dc_motor.stop()
    sleep(5)
    print('Backwards with speed: 100%')
    dc_motor.backwards(100)
    sleep(5)
    print('Forward with speed: 5%')
    dc_motor.forward(5)
    sleep(5)
    dc_motor.stop()
    
except KeyboardInterrupt:
    print('Keyboard Interrupt')
    dc_motor.stop()
