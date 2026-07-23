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
#define DELAY_MS     500

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DIN_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));     // Color » Red
  FastLED.show();
  delay(DELAY_MS);

  fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));     // Color » Green
  FastLED.show();
  delay(DELAY_MS);

  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));     // Color » Blue
  FastLED.show();
  delay(DELAY_MS);

  fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255)); // Color » White
  FastLED.show();
  delay(DELAY_MS);
}
