/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-max30102-oximeter-heart-rate-sensor/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 sensor;

// Settings
const byte RATE_SIZE = 5;   // How many beats we average
byte rates[RATE_SIZE];      // Store last few valid BPM values
byte rateSpot = 0;
byte validBeats = 0;
long lastBeat = 0;
float currentBPM = 0;
int averageBPM = 0;

unsigned long lastPrint = 0;

// Only accept realistic heart rates (adjust for your case)
const float MIN_BPM = 45;
const float MAX_BPM = 180;

void setup() {
  Serial.begin(115200);
  Serial.println("MAX30102 Heart Rate Sensor");
  Serial.println();
  Serial.println("Place your finger on the sensor...");
  Serial.println();

  // Initialize the sensor
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Error initializing the sensor...");
    while (1);
  }

  sensor.setup();                    // Use default settings
  sensor.setPulseAmplitudeRed(0);    // Turn off red LED
}

void loop() {
  long irValue = sensor.getIR();      // Read infrared value

  // Check if a heartbeat was detected
  if (checkForBeat(irValue)) {
    long timeBetweenBeats = millis() - lastBeat;
    lastBeat = millis();

    currentBPM = 60.0 / (timeBetweenBeats / 1000.0);

    // Only keep realistic values
    if (currentBPM >= MIN_BPM && currentBPM <= MAX_BPM) {
      rates[rateSpot] = (byte)currentBPM;
      rateSpot++;
      if (rateSpot >= RATE_SIZE){
        rateSpot = 0;
      }
      // Count valid beats (until buffer is full)
      if (validBeats < RATE_SIZE) {
        validBeats++;
      }
      
      // Only calculate average when buffer is full
      if (validBeats >= RATE_SIZE) {
        // Calculate the average
        averageBPM = 0;
        for (byte i = 0; i < RATE_SIZE; i++) {
          averageBPM += rates[i];
        }
        averageBPM /= RATE_SIZE;
      }
    }
  }

  // Print once every second
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

    if (irValue < 50000) {
      // No finger on the sensor
      Serial.println("Waiting for finger...");
    }
    else if (averageBPM == 0) {
      // Finger is present but not enough valid beats yet
      Serial.println("Measuring... keep your finger still");
    }
    else {
      // We have a valid average
      Serial.print("Heart Rate: ");
      Serial.print(averageBPM);
      Serial.println(" BPM");
    }
  }
}
