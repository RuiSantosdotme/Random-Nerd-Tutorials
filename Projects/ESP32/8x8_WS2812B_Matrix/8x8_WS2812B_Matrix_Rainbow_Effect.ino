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

// Rainbow animation 
#define BASE_HUE           160    // Initial blue hue for new LED (0-255)
#define HUE_STEP_PER_TICK  3      // Amount the LED hue shifts per frame
#define DELAY_MS           80     // Animation speed
uint16_t ledsLit = 0;             // Number of LEDs currently turned on
uint16_t frame = 0;               // Counts animation frames

CRGB leds[NUM_LEDS];

// Convert (x,y) coordinates to the corresponding index in the matrix
uint16_t coordToIndex(uint8_t x, uint8_t y) {
  if (ZIGZAG && (y & 0x01)) {
    x = (MATRIX_WIDTH - 1) - x;
  }
  return (y * MATRIX_WIDTH) + x;
}

// Set the color of a pixel at (x,y)
void setPixel(uint8_t x, uint8_t y, CRGB color, uint8_t brightness = 255) {
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) 
      return;
    
    color.nscale8(brightness);
    leds[coordToIndex(x, y)] = color;
}

void setup() {
  FastLED.addLeds<LED_TYPE, DIN_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // Turns on 1 LED per frame until fully lit
  if (ledsLit < NUM_LEDS) {
    ledsLit++;
  }

  // Update colors of all currently lit LEDs
  for (uint16_t i = 0; i < ledsLit; i++) {
    uint8_t x = i % MATRIX_WIDTH;
    uint8_t y = i / MATRIX_WIDTH;

    // Calculate hue based on how long this LED has been lit
    // Newer LEDs start at BASE_HUE, older ones shift further along the color wheel
    uint16_t age = frame - i;
    uint8_t  hue = BASE_HUE + (uint8_t)(age * HUE_STEP_PER_TICK);

    setPixel(x, y, CHSV(hue, 255, 255));
  }

  FastLED.show();
  delay(DELAY_MS);
  frame++;
}
