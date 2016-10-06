/*
 * Random Nerd Tutorials - Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 * 
 * The MIT License (MIT)
 * Copyright (c) 2016 by Daniel Eichhorn
 */
 
// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
// or #include "SH1106.h" alis for `#include "SH1106Wire.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

#include <DHT.h>

#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D3, D5);
// SH1106 display(0x3c, D3, D5);

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  dht.begin(); // initialize dht
}

void displayTempHumid(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)){
    display.clear(); // clearing the display
    display.drawString(5,0, "Failed DHT");
    return;
  }
  display.clear();
  display.drawString(0, 0, "Humidity: " + String(h) + "%\t"); 
  display.drawString(0, 16, "Temp: " + String(t) + "C"); 
  display.drawString(0, 32, "Temp: " + String(f) + "F"); 
}
void loop(){
  displayTempHumid();
  display.display();
  delay(2000);
}
