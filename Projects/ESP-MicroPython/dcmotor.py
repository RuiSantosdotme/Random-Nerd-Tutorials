# Created by https://RandomNerdTutorials.com/micropython-esp32-esp8266-dc-motor-l298n/
#This file includes a class to control DC motors

class DCMotor:
  #the min_duty and max_duty are defined for 15000Hz frequency
  #you can pass as arguments
  def __init__(self, pin1, pin2, enable_pin, min_duty=750, max_duty=1023):
    self.pin1 = pin1
    self.pin2= pin2
    self.enable_pin = enable_pin
    self.min_duty = min_duty
    self.max_duty = max_duty
  
  #speed value can be between 0 and 100
  def forward(self, speed):
    self.speed = speed
    self.enable_pin.duty(self.duty_cycle(self.speed))
    self.pin1.value(1)
    self.pin2.value(0)

  def backwards(self, speed):
    self.speed = speed
    self.enable_pin.duty(self.duty_cycle(self.speed))
    self.pin1.value(0)
    self.pin2.value(1)

  def stop(self):
    self.enable_pin.duty(0)
    self.pin1.value(0)
    self.pin2.value(0)
        
  def duty_cycle(self, speed):
    if self.speed <= 0 or self.speed > 100:
      duty_cycle = 0
    else:
      duty_cycle = int (self.min_duty + (self.max_duty - self.min_duty)*((self.speed - 1)/(100-1)))
    return duty_cycle