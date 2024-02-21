# Complete project details at https://RandomNerdTutorials.com/micropython-esp32-esp8266-dc-motor-l298n/

from dcmotor import DCMotor
from machine import Pin, PWM
from time import sleep

frequency = 15000

pin1 = Pin(12, Pin.OUT)
pin2 = Pin(14, Pin.OUT)
enable = PWM(Pin(13), frequency)

dc_motor = DCMotor(pin1, pin2, enable)
#Set min duty cycle (350) and max duty cycle (1023)
#dc_motor = DCMotor(pin1, pin2, enable, 350, 1023)

dc_motor.forward(50)
sleep(2)
dc_motor.stop()
sleep(3)
dc_motor.backwards(100)
sleep(2)
dc_motor.forward(5)
sleep(5)
dc_motor.stop()