/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/arduino-neo-m8n-gps-module/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <SoftwareSerial.h>

static const int RXPin = 6, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// The serial connection to the GPS module
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup(){
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
}

void loop(){
  while (gpsSerial.available() > 0){
    // get the byte data from the GPS
    byte gpsData = gpsSerial.read();
    Serial.write(gpsData);
  }
}
