// Complete project details at https://RandomNerdTutorials.com/esp8266-load-cell-hx711/
// Library HX711 by Bogdan Necula: https://github.com/bogde/HX711
// Library: pushbutton by polulu: https://github.com/pololu/pushbutton-arduino

#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Pushbutton.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;

HX711 scale;
int reading;
int lastReading;
//REPLACE WITH YOUR CALIBRATION FACTOR
#define CALIBRATION_FACTOR -478.507

//OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Button
#define BUTTON_PIN 14
Pushbutton button(BUTTON_PIN);

void displayWeight(int weight){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Weight:");
  display.display();
  display.setCursor(0, 30);
  display.setTextSize(2);
  display.print(weight);
  display.print(" ");
  display.print("g");
  display.display();  
}

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.set_scale(CALIBRATION_FACTOR);   // this value is obtained by calibrating the scale with known weights
  scale.tare();               // reset the scale to 0
}

void loop() {
  
  if (button.getSingleDebouncedPress()){
    Serial.print("tare...");
    scale.tare();
  }
  
  if (scale.wait_ready_timeout(200)) {
    reading = round(scale.get_units());
    Serial.print("Weight: ");
    Serial.println(reading);
    if (reading != lastReading){
      displayWeight(reading); 
    }
    lastReading = reading;
  }
  else {
    Serial.println("HX711 not found.");
  }
}
