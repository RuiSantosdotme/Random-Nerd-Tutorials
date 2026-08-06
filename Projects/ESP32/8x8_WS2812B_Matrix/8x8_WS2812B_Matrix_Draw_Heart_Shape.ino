/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-64-ws2812b-8x8-matrix-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <FastLED.h>

#define DIN_PIN       2
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8
#define NUM_LEDS      (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS    80         // Brightness (0-255)
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB
#define ZIGZAG        true       // Set false if your panel is wired in straight rows instead of zigzag
#define FLIP_H        false      // Set true if the image displays mirrored left to right
#define FLIP_V        false      // Set true if the image displays upside down

CRGB leds[NUM_LEDS];

// Heart shape (8x8)
const bool heart[MATRIX_HEIGHT][MATRIX_WIDTH] = {
  {0,0,0,0,0,0,0,0},
  {0,1,1,0,0,1,1,0},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {0,1,1,1,1,1,1,0},
  {0,0,1,1,1,1,0,0},
  {0,0,0,1,1,0,0,0}
};

// Convert (x,y) coordinates to the corresponding index in the matrix
uint16_t coordToIndex(uint8_t x, uint8_t y) {
  if (FLIP_H) x = (MATRIX_WIDTH - 1) - x;
  if (FLIP_V) y = (MATRIX_HEIGHT - 1) - y;

  if (ZIGZAG && (y & 0x01)) {
    x = (MATRIX_WIDTH - 1) - x;
  }
  return (y * MATRIX_WIDTH) + x;
}

// Set the color of a pixel at (x,y)
void setPixel(uint8_t x, uint8_t y, CRGB color) {
  if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) 
    return;
  leds[coordToIndex(x, y)] = color;
}

// Draws the heart shape - scales its color by a brightness factor (0.0 - 1.0)
void drawShape(CRGB color, float scale) {
  FastLED.clear();
  CRGB c = color;
  c.nscale8_video((uint8_t)(scale * 255));
  for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      if (heart[y][x]) setPixel(x, y, c);
    }
  }
}

// Pulses a heartrate LED pattern by changing the brightness over durationMs
// It creates a fade in and fade out effect
void pulse(CRGB color, float from, float to, uint16_t durationMs) {
  const uint8_t steps = 24;
  for (uint8_t i = 0; i <= steps; i++) {
    float t = (float)i / steps;
    float eased = (1 - cos(t * PI)) / 2.0;
    float scale = from + (to - from) * eased;
    drawShape(color, scale);
    FastLED.show();
    delay(durationMs / steps);
  }
}

void setup() {
  FastLED.addLeds<LED_TYPE, DIN_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // Red color
  CRGB color = CRGB(255, 0, 0);
  // Heartbeat animation
  pulse(color, 0.30, 1.00, 120);
  pulse(color, 1.00, 0.55, 140);
  pulse(color, 0.55, 0.85, 100);
  pulse(color, 0.85, 0.30, 160);
  drawShape(color, 0.30);
  FastLED.show();
  delay(300);
}
