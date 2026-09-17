/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-max30102-oximeter-heart-rate-sensor/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 sensor;

// Buffers needed by the algorithm
#define BUFFER_SIZE 100
uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];

int32_t spo2;          // Oxygen saturation value
int8_t  validSPO2;     // 1 = valid reading, 0 = not valid yet
int32_t heartRate;     // Not used, but required by the function
int8_t  validHeartRate;

void setup() {
  Serial.begin(115200);
  Serial.println("MAX30102 Oxygen Saturation");
  Serial.println();
  Serial.println("Place your finger on the sensor...");
  Serial.println();

  // Initialize the sensor
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Error initializing the sensor...");
    while (1);
  }

  byte ledBrightness = 60;  // 0=Off to 255=50mA
  byte sampleAverage = 4;   // 1, 2, 4, 8, 16, 32
  byte ledMode = 2;         // 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100;    // 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;     // 69, 118, 215, 411
  int adcRange = 4096;      // 2048, 4096, 8192, 16384

  // Configure sensor with these settings
  sensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop() {
  // Collect 100 samples (takes a few seconds)
  for (byte i = 0; i < BUFFER_SIZE; i++) {
    while (sensor.available() == false) {
      sensor.check();
    }

    redBuffer[i] = sensor.getRed();
    irBuffer[i]  = sensor.getIR();
    sensor.nextSample();
  }

  // Calculate SpO2
  maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_SIZE, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Display oxygen saturation
  if (validSPO2) {
    Serial.print("Oxygen Saturation: ");
    Serial.print(spo2);
    Serial.println(" %");
  } else {
    Serial.println("Measuring... keep your finger still");
  }

  Serial.println();
}
