/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-apds9960-sensor-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include "Adafruit_APDS9960.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// APDS9960 sensor
Adafruit_APDS9960 apds;

// Function to center text horizontally
void centerText(const char* text, int y) {
  int charCount = strlen(text);
  int textWidth = charCount * 6;
  int x = (SCREEN_WIDTH - textWidth) / 2;
  display.setCursor(x, y);
  display.println(text);
}

// Function to display UP gesture
void displayScreenUp() {
  display.clearDisplay();
  centerText("^", 10);
  centerText("Moving UP", 30);
  display.display();
  Serial.println("^ Moving UP");
}

// Function to display DOWN gesture
void displayScreenDown() {
  display.clearDisplay();
  centerText("v", 10);
  centerText("Moving DOWN", 30);
  display.display();
  Serial.println("v Moving DOWN");
}

// Function to display LEFT gesture
void displayScreenLeft() {
  display.clearDisplay();
  centerText("<", 10);
  centerText("Moving LEFT", 30);
  display.display();
  Serial.println("< Moving LEFT");
}

// Function to display RIGHT gesture
void displayScreenRight() {
  display.clearDisplay();
  centerText(">", 10);
  centerText("Moving RIGHT", 30);
  display.display();
  Serial.println("> Moving RIGHT");
}

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the display
  display.clearDisplay();
  display.display();
  delay(500);

  display.setTextSize(1);      
  display.setTextColor(WHITE);
  display.setCursor(0,0); 
  display.println(F("Gesture Sensor"));
  display.display();
  delay(100);
  
  // Initialize APDS9960
  if(!apds.begin()){
    Serial.println("Failed to initialize APDS9960! Check wiring.");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("APDS9960 Failed"));
    display.display();
  } else {
    Serial.println("APDS9960 initialized!");
    apds.enableProximity(true);
    apds.enableGesture(true);
  }
}

void loop() {
  uint8_t gesture = apds.readGesture();
  if (gesture == APDS9960_DOWN) {
    displayScreenDown();
  }
  if (gesture == APDS9960_UP) {
    displayScreenUp();
  }
  if (gesture == APDS9960_LEFT) {
    displayScreenLeft();
  }
  if (gesture == APDS9960_RIGHT) {
    displayScreenRight();
  }
}