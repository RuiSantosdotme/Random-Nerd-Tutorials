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
  leds[0] = CRGB(255, 0, 0);     // Red » (R=255, G=0, B=0)
  FastLED.show();
  delay(DELAY_MS);

  leds[1] = CRGB(0, 255, 0);     // Green » (R=0, G=255, B=0)
  FastLED.show();
  delay(DELAY_MS);

  leds[2] = CRGB(0, 0, 255);     // Blue » (R=0, G=0, B=255)
  FastLED.show();
  delay(DELAY_MS);

  leds[3] = CRGB(255, 165, 0);   // Orange » (R=255, G=165, B=0)
  FastLED.show();
  delay(DELAY_MS);

  leds[4] = CRGB(128, 0, 128);   // Purple » (R=128, G=0, B=128)
  FastLED.show();
  delay(DELAY_MS);

  leds[5] = CRGB(255, 255, 0);   // Yellow » (R=255, G=255, B=0)
  FastLED.show();
  delay(DELAY_MS);
  
  leds[6] = CRGB(255, 255, 255); // White » (R=255, G=255, B=255)
  FastLED.show();
  delay(DELAY_MS);

  leds[7] = CRGB(0, 0, 0);       // Off/Black » (R=0, G=0, B=0)
  FastLED.show();
  delay(DELAY_MS);

  FastLED.clear();
  delay(DELAY_MS);
}
