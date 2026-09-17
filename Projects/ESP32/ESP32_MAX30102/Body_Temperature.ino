/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-max30102-oximeter-heart-rate-sensor/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Wire.h>
#include "MAX30105.h"

MAX30105 sensor;

void setup() {
  Serial.begin(115200);
  Serial.println("MAX30102 Temperature Sensor");
  Serial.println();

  // Initialize the sensor
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Error initializing the sensor...");
    while (1);
  }
  
  // Turn off the LEDs so they don't heat the sensor
  sensor.setup(0);            // 0 = LEDs off
  sensor.enableDIETEMPRDY();  // Enable temperature ready signal
}

void loop() {
  // Read temperature in Celsius
  float tempC = sensor.readTemperature();

  // Convert to Fahrenheit
  float tempF = tempC * 1.8 + 32.0;

  // Print the temperature readings
  Serial.print("Temperature: ");
  Serial.print(tempC, 2);
  Serial.print(" °C   |   ");
  Serial.print(tempF, 2);
  Serial.println(" °F");

  delay(1000);
}
