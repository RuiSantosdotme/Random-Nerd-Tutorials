/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-sht41-temperature-humidity-arduino/
  Based on the example provided by the Adafruit_SHT4x Library
*/
#include "Adafruit_SHT4x.h"

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

void setup() {
  Serial.begin(115200);

  Serial.println("SHT4x test");
  if (!sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision()) {
     case SHT4X_HIGH_PRECISION: 
       Serial.println("High precision");
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println("Med precision");
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println("Low precision");
       break;
  }

  // You can have 6 different heater settings higher heat and 
  // longer times uses more power and reads will take longer too
  sht4.setHeater(SHT4X_NO_HEATER);
  switch (sht4.getHeater()) {
     case SHT4X_NO_HEATER: 
       Serial.println("No heater");
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println("High heat for 0.1 second");
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println("Medium heat for 1 second");
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println("Medium heat for 0.1 second");
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println("Low heat for 1 second");
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println("Low heat for 0.1 second");
       break;
  }
}

void loop() {
  sensors_event_t humidity, temp;
  
  uint32_t timestamp = millis();
  // Get the latest temperature and humidity readings
  sht4.getEvent(&humidity, &temp);
  timestamp = millis() - timestamp;

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degrees C");

  Serial.print("Humidity: "); 
  Serial.print(humidity.relative_humidity);
  Serial.println("% rH");
  
  // Convert temperature to Fahrenheit
  float tempF = 1.8 * temp.temperature + 32;
  Serial.print("Temperature: ");
  Serial.print(tempF);
  Serial.println(" degrees F");

  Serial.print("Read duration (ms): ");
  Serial.println(timestamp);
  Serial.println(" ");

  delay(1000);
}
