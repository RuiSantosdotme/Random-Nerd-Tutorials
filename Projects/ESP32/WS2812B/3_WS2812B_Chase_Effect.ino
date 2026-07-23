/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-ws2812b-neopixels-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <FastLED.h>

#define NUM_LEDS     8
#define DIN_PIN      2
#define BRIGHTNESS   80   // Brightness (0-255)
#define DELAY_MS     50   // Chase effect speed

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DIN_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  // Chase effect from left to right
  for (int i = 0; i < NUM_LEDS; i++) {
    FastLED.clear();            // Clear LEDs
    leds[i] = CRGB(0, 0, 255);  // Light current position
    FastLED.show();
    delay(DELAY_MS);            // Chase effect speed
  }
  // Reverse the chase effect direction
  for (int i = NUM_LEDS - 1; i >= 0; i--) { 
    FastLED.clear();
    leds[i] = CRGB(0, 0, 255);
    FastLED.show();
    delay(DELAY_MS);
  }
}
