# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-pwm-micropython/
from machine import Pin, PWM
from time import sleep

# Set up PWM Pin
led = machine.Pin(20)
led_pwm = PWM(led)
duty_step = 129  # Step size for changing the duty cycle

#Set PWM frequency
frequency = 5000
led_pwm.freq (frequency)

try:
    while True:
      # Increase the duty cycle gradually
      for duty_cycle in range(0, 65536, duty_step):
        led_pwm.duty_u16(duty_cycle)
        sleep(0.005)
        
      # Decrease the duty cycle gradually
      for duty_cycle in range(65536, 0, -duty_step):
        led_pwm.duty_u16(duty_cycle)
        sleep(0.005)
        
except KeyboardInterrupt:
    print("Keyboard interrupt")
    led_pwm.duty_u16(0)
    print(led_pwm)
    led_pwm.deinit()
