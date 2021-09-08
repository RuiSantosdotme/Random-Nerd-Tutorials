/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/tca9548a-i2c-multiplexer-esp32-esp8266-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1022)

Adafruit_BME280 bme1; // I2C
Adafruit_BME280 bme2; // I2C
Adafruit_BME280 bme3; // I2C
Adafruit_BME280 bme4; // I2C

// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}

void printValues(Adafruit_BME280 bme, int bus) {
  TCA9548A (bus);
  Serial.print("Sensor number on bus");
  Serial.println(bus);
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void setup() {
  Serial.begin(115200);
  
  // Start I2C communication with the Multiplexer
  Wire.begin();

  // Init sensor on bus number 2
  TCA9548A(2);
  if (!bme1.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor on bus 2, check wiring!");
    while (1);
  }
  Serial.println();
  
  // Init sensor on bus number 3
  TCA9548A(3);
  if (!bme2.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor on bus 2, check wiring!");
    while (1);
  }
  Serial.println();
  
  // Init sensor on bus number 4
  TCA9548A(4);
  if (!bme3.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor on bus 2, check wiring!");
    while (1);
  }
  Serial.println();
  
  // Init sensor on bus number 5
  TCA9548A(5);
  if (!bme4.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor on bus 2, check wiring!");
    while (1);
  }
  Serial.println();
}

void loop() { 
  //Print values for sensor 1
  printValues(bme1, 2);
  printValues(bme2, 3);
  printValues(bme3, 4);
  printValues(bme4, 5);
  delay(5000);
}
