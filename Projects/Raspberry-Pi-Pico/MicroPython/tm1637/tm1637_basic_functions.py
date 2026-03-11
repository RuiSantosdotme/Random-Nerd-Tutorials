# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-tm1637-micropython/

import tm1637
from machine import Pin
from time import sleep

# Initialize display (adjust pins if needed)
display = tm1637.TM1637(clk=Pin(21), dio=Pin(20))

# Set display brightness
display.brightness(7)

while True:
    # all LEDS on "88:88"
    display.write([127, 255, 127, 127])
    sleep(1)

    # all LEDS off
    display.write([0, 0, 0, 0])
    sleep(1)

    # show "0123"
    display.write([63, 6, 91, 79])
    sleep(1)

    # show "COOL"
    display.write([0b00111001, 0b00111111, 0b00111111, 0b00111000])
    sleep(1)

    # show "HELP"
    display.show('help')
    sleep(1)

    # display "dEAd", "bEEF"
    display.hex(0xdead)
    sleep(1)
    display.hex(0xbeef)
    sleep(1)

    # show "12:59"
    display.numbers(12, 59)
    sleep(1)

    # show "-123"
    display.number(-123)
    sleep(1)

    # show temperature '24*C'
    display.temperature(24)
    sleep(1)
    
    # show temperature '75*F'
    display.temperature_f(75)
    sleep(1)

    # display scrolling text
    display.scroll('Random Nerd Tutorials', delay=500)
    sleep(1)
