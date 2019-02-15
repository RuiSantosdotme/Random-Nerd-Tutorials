# Complete project details at https://RandomNerdTutorials.com

from machine import Pin
import machine, neopixel, time

# define interrupt handling functions
def button_handler(pin):
  global button_pressed
  button_pressed = pin

# configure pushbuttons as interrupts
button1 = Pin(15, Pin.IN)
button1.irq(trigger=Pin.IRQ_RISING, handler=button_handler)
button2 = Pin(14, Pin.IN)
button2.irq(trigger=Pin.IRQ_RISING, handler=button_handler)
button3 = Pin(12, Pin.IN)
button3.irq(trigger=Pin.IRQ_RISING, handler=button_handler)
button4 = Pin(13, Pin.IN)
button4.irq(trigger=Pin.IRQ_RISING, handler=button_handler)

button_pressed = button1

# LED strip configuration
# number of pixels
n = 48
# strip control gpio
p = 5 
np = neopixel.NeoPixel(machine.Pin(p), n)

# FUNCTIONS FOR LIGHTING EFFECTS
# bounce
def bounce(r, g, b, wait):
  for i in range(2 * n):
    for j in range(n):
      np[j] = (r, g, b)
    if (i // n) % 2 == 0:
      np[i % n] = (0, 0, 0)
    else:
      np[n - 1 - (i % n)] = (0, 0, 0)
    np.write()
    time.sleep_ms(wait)

# cycle
def cycle(r, g, b, wait):
  for i in range(n):
    for j in range(n):
      np[j] = (0, 0, 0)
    np[i % n] = (r, g, b)
    np.write()
    time.sleep_ms(wait)

# function to go through all colors 
def wheel(pos):
  # Input a value 0 to 255 to get a color value.
  # The colours are a transition r - g - b - back to r.
  if pos < 0 or pos > 255:
    return (0, 0, 0)
  if pos < 85:
    return (255 - pos * 3, pos * 3, 0)
  if pos < 170:
    pos -= 85
    return (0, 255 - pos * 3, pos * 3)
  pos -= 170
  return (pos * 3, 0, 255 - pos * 3)

# rainbow 
def rainbow_cycle(wait):
  for j in range(255):
    for i in range(n):
      rc_index = (i * 256 // n) + j
      np[i] = wheel(rc_index & 255)
    np.write()
    time.sleep_ms(wait)

# turn off all pixels
def clear():
  for i in range(n):
    np[i] = (0, 0, 0)
    np.write()

while True:
  if button_pressed == button1:
    clear()
  elif button_pressed == button2:
    bounce(23, 210, 15, 70)
  elif button_pressed == button3:
    cycle(123, 0, 154, 50)
  elif button_pressed == button4:
    rainbow_cycle(1)
