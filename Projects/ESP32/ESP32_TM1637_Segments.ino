/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-tm1637-4-digit-7-segment-display-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 19
#define DIO 18
TM1637Display display(CLK, DIO);

void setup() {
  display.setBrightness(7); 

  // Custom segments (one byte per digit)
  uint8_t segments[] = {
    0b0111111, // 0 — all segments except G
    0b0000110, // 1 — segments B and C
    0b1011011, // 2
    0b1001111  // 3
  };

  display.setSegments(segments);
}

void loop() {
  
}
