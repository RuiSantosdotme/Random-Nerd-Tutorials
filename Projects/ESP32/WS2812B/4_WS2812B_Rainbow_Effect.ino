/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-ws2812b-neopixels-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <FastLED.h>

#define NUM_LEDS     8
#define DIN_PIN      2
#define SATURATION   255   // Saturarion (0-255)
#define BRIGHTNESS   255   // Brightness (0-255)
#define DELAY_MS     20 

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DIN_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set LED i to a moving rainbow color effect: i*10 creates color spread across LEDs
    // millis()/20 makes the rainbow slowly animate over time with full saturation and full brightness
    leds[i] = CHSV(i * 10 + millis() / 20, SATURATION, BRIGHTNESS);
  }
  FastLED.show();
  delay(DELAY_MS);
}
