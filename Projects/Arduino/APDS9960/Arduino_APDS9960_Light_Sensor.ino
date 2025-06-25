/* This is a library for the APDS9960 digital proximity, ambient light, RGB, and gesture sensor
   This sketch puts the sensor in color mode and reads the RGB and clear values.
   Designed specifically to work with the Adafruit APDS9960 breakout ----> http://www.adafruit.com/products/3595
   These sensors use I2C to communicate. The device's I2C address is 0x39 Adafruit invests time and resources providing this open source code, please support Adafruit andopen-source hardware by purchasing products from Adafruit! Written by Dean Miller for Adafruit Industries. BSD license, all text above must be included in any redistribution */

// Complete project details at https://RandomNerdTutorials.com/arduino-apds9960-gesture-sensor/

#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

void setup() {
  Serial.begin(115200);

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //enable color sensing mode
  apds.enableColor(true);

  // Set ADC gain to high (16x) for better color sensitivity
  apds.setADCGain(APDS9960_AGAIN_16X);
}

void loop() {
  //create some variables to store the color data in
  uint16_t r, g, b, c;
  
  //wait for color data to be ready
  while(!apds.colorDataReady()){
    delay(5);
  }

  //get the data and print the different channels
  apds.getColorData(&r, &g, &b, &c);
  Serial.print("Red Light: ");
  Serial.print(r);
  
  Serial.print(" || Green Light: ");
  Serial.print(g);
  
  Serial.print(" || Blue Light: ");
  Serial.print(b);
  
  Serial.print(" || Ambient Light: ");
  Serial.println(c);
  Serial.println();
  
  delay(500);
}