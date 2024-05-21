# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-dc-motor-micropython/

class DCMotor:
    def __init__(self, pin1, pin2, enable_pin, min_duty=15000, max_duty=65535):
        self.pin1 = pin1
        self.pin2 = pin2
        self.enable_pin = enable_pin
        self.min_duty = min_duty
        self.max_duty = max_duty

    def forward(self, speed):
        self.speed = speed
        self.enable_pin.duty_u16(self.duty_cycle(self.speed))
        self.pin1.value(1)
        self.pin2.value(0)

    def backwards(self, speed):
        self.speed = speed
        self.enable_pin.duty_u16(self.duty_cycle(self.speed))
        self.pin1.value(0)
        self.pin2.value(1)

    def stop(self):
        self.enable_pin.duty_u16(0)
        self.pin1.value(0)
        self.pin2.value(0)

    def duty_cycle(self, speed):
        if speed <= 0 or speed > 100:
            duty_cycle = 0
        else:
            duty_cycle = int(self.min_duty + (self.max_duty - self.min_duty) * (speed / 100))
        return duty_cycle
