/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-64-ws2812b-8x8-matrix-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <FastLED.h>

#define DIN_PIN        2
#define MATRIX_WIDTH   8
#define MATRIX_HEIGHT  8
#define NUM_LEDS       (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS     80          // Brightness (0-255)
#define LED_TYPE       WS2812B
#define COLOR_ORDER    GRB
#define ZIGZAG         true        // Set false if your panel is wired in straight rows instead of zigzag
#define DELAY_MS       500

CRGB leds[NUM_LEDS];

// Convert (x,y) coordinates to the corresponding index in the matrix
uint16_t coordToIndex(uint8_t x, uint8_t y) {
  if (ZIGZAG && (y & 0x01)) {
    x = (MATRIX_WIDTH - 1) - x;
  }
  return (y * MATRIX_WIDTH) + x;
}

// Set the color of a pixel at (x,y)
void setPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 255) {
  if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) 
    return;
  CRGB color = CRGB(r, g, b);
  color.nscale8(brightness);
  leds[coordToIndex(x, y)] = color;
}

void setup() {
  FastLED.addLeds<LED_TYPE, DIN_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  setPixel(0, 0, 255, 0, 0);         // First LED x=0 and y=0  |  Red - Full brightness
  FastLED.show();
  delay(DELAY_MS);

  setPixel(3, 3,  0, 255, 0, 128);   // LED x=3 and y=3  |  Green - 50% brightness
  FastLED.show();
  delay(DELAY_MS);

  setPixel(7, 7, 0, 0, 255, 200);    // Last LED x=7 and y=7  |  Blue - ~78% brightness
  FastLED.show();
  delay(DELAY_MS);

  FastLED.clear();                   // All LEDs off
  FastLED.show();
  delay(DELAY_MS);
}
