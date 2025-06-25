/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/arduino-apds9960-gesture-sensor/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <Wire.h>

// Define APDS9960 chip ID register
#define APDS9960_ID_REG 0x92 // Chip ID register address

// Function to read chip ID from a given I2C address
uint8_t readChipID(uint8_t i2cAddress) {
  Wire.beginTransmission(i2cAddress);
  Wire.write(APDS9960_ID_REG); // Request chip ID register
  Wire.endTransmission(false); 
  Wire.requestFrom(i2cAddress, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read(); // Read chip ID
  }
  return 0; // Return 0 if no data (invalid or no response)
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting I2C Scanner and Chip ID Reader");

  // Initialize I2C
  Wire.begin();
  Wire.setClock(100000); 

  // Scan I2C bus for devices
  Serial.println("\nScanning I2C bus...");
  bool deviceFound = false;

  for (uint8_t address = 0x08; address <= 0x7F; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      deviceFound = true;
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);

      // Read and print chip ID
      uint8_t chipID = readChipID(address);
      Serial.print(" - Chip ID: 0x");
      if (chipID < 16) Serial.print("0");
      Serial.println(chipID, HEX);
    }
  }

  if (!deviceFound) {
    Serial.println("No I2C devices found. Check wiring or try again.");
  } else {
    Serial.println("\nScan complete.");
  }
}

void loop() {
}