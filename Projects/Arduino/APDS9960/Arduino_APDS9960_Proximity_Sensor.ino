/* This is a library for the APDS9960 digital proximity, ambient light, RGB, and gesture sensor
   This sketch puts the sensor in proximity mode and enables the interrupt to fire when proximity goes over a set value
   Designed specifically to work with the Adafruit APDS9960 breakout ----> http://www.adafruit.com/products/3595
   These sensors use I2C to communicate. The device's I2C address is 0x39 Adafruit invests time and resources providing this open source code, please support Adafruit andopen-source hardware by purchasing products from Adafruit! Written by Dean Miller for Adafruit Industries. BSD license, all text above must be included in any redistribution */

// Complete project details at https://RandomNerdTutorials.com/arduino-apds9960-gesture-sensor/

#include "Adafruit_APDS9960.h"

//the pin that the interrupt is attached to
#define INT_PIN 3

// LED for visual output
#define LED_PIN 7

//create the APDS9960 object
Adafruit_APDS9960 apds;

void setup() {
  Serial.begin(115200);
  pinMode(INT_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //enable proximity mode
  apds.enableProximity(true);

  //set the interrupt threshold to fire when proximity reading goes above 175
  apds.setProximityInterruptThreshold(0, 175);

  //enable the proximity interrupt
  apds.enableProximityInterrupt();
}

void loop() {

  //print the proximity reading when the interrupt pin goes low
  if(!digitalRead(INT_PIN)){
    Serial.println(apds.readProximity());
    digitalWrite(LED_PIN, HIGH);

    //clear the interrupt
    apds.clearInterrupt();
  }
  digitalWrite(LED_PIN, LOW);
}