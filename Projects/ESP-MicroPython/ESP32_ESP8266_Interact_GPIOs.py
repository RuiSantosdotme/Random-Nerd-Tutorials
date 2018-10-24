# Complete project details at https://RandomNerdTutorials.com
# Created by Rui Santos

from machine import Pin, ADC, PWM
from time import sleep

led = Pin(2, Pin.OUT)
button = Pin(15, Pin.IN)

#Configure ADC for ESP32
pot = ADC(Pin(34))
pot.width(ADC.WIDTH_10BIT)
pot.atten(ADC.ATTN_11DB)

#Configure ADC for ESP8266
#pot = ADC(0)

led_pwm = PWM(Pin(4),5000)

while True:
  button_state = button.value()
  led.value(button_state)

  pot_value = pot.read()
  led_pwm.duty(pot_value)

  sleep(0.1)
