/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/tca9548a-i2c-multiplexer-esp32-esp8266-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  Serial.print(bus);
}
 
void setup() {
  Serial.begin(115200);

  // Start I2C communication with the Multiplexer
  Wire.begin();

  // Init OLED display on bus number 2
  TCA9548A(2);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
  // Clear the buffer
  display.clearDisplay();

  // Init OLED display on bus number 3
  TCA9548A(3);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
  // Clear the buffer
  display.clearDisplay();

  // Init OLED display on bus number 4
  TCA9548A(4);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
  // Clear the buffer
  display.clearDisplay();

  // Init OLED display on bus number 5
  TCA9548A(5);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
  // Clear the buffer
  display.clearDisplay();

  // Write to OLED on bus number 2
  TCA9548A(2);
  display.clearDisplay();
  display.setTextSize(8);
  display.setTextColor(WHITE);
  display.setCursor(45, 10);
  // Display static text
  display.println("1");
  display.display(); 

  // Write to OLED on bus number 3
  TCA9548A(3);
  display.clearDisplay();
  display.setTextSize(8);
  display.setTextColor(WHITE);
  display.setCursor(45, 10);
  // Display static text
  display.println("2");
  display.display(); 
  
  // Write to OLED on bus number 4
  TCA9548A(4);
  display.clearDisplay();
  display.setTextSize(8);
  display.setTextColor(WHITE);
  display.setCursor(45, 10);
  // Display static text
  display.println("3");
  display.display(); 
  
  // Write to OLED on bus number 5
  TCA9548A(5);
  display.clearDisplay();
  display.setTextSize(8);
  display.setTextColor(WHITE);
  display.setCursor(45, 10);
  // Display static text
  display.println("4");
  display.display(); 
}
 
void loop() {
  
}
