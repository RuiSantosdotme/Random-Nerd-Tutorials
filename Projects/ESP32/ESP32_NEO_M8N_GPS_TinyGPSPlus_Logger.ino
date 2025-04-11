/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-neo-m8n-gps-logger-google-earth/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <TinyGPS++.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// Define the RX and TX pins for Serial 2
#define RXD2 16
#define TXD2 17

#define GPS_BAUD 9600

// The TinyGPS++ object
TinyGPSPlus gps;

// Create an instance of the HardwareSerial class for Serial 2
HardwareSerial gpsSerial(2);

String GPSdataToAppend;

// Previous GPS coordinates
double lastLat = 0.0;
double lastLng = 0.0;

// Initialize SD card
void initSDCard() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void setup() {
  // Serial Monitor
  Serial.begin(115200);

  // Initialize the microSD card
  initSDCard();

  // Start Serial 2 with the defined RX and TX pins and a baud rate of 9600
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial 2 started at 9600 baud rate");
}

void loop() {
  unsigned long start = millis();

  while (millis() - start < 1000) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isUpdated()) {
      double currentLat = gps.location.lat();
      double currentLng = gps.location.lng();

      // Check if the distance from the last point is at least 1 meter
      double distance = TinyGPSPlus::distanceBetween(lastLat, lastLng, currentLat, currentLng);

      if (distance >= 1.0) {
        lastLat = currentLat;
        lastLng = currentLng;

        // Prepare data to append
        GPSdataToAppend = String(currentLng, 6) + "," + String(currentLat, 6) + "," + String(gps.altitude.meters());
        Serial.println(GPSdataToAppend);

        // Append to file
        appendFile(SD, "/data.txt", GPSdataToAppend.c_str());
      }
    }
  }
}
