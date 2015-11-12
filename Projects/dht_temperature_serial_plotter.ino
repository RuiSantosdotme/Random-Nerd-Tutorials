// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada modified by Rui Santos

#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
  dht.begin();  
}

void loop() {
  // Read temperature as Celsius
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    return;
  } 

  // Prints temperature in the Arduino Plotter Tool
  Serial.println(t);

   // Wait a few seconds between measurements.
  delay(10);
}
