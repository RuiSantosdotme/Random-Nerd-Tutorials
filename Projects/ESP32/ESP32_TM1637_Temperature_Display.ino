/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-tm1637-4-digit-7-segment-display-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TM1637Display.h>

// Wi-Fi credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

const char* api_key = "REPLACE_WITH_YOUR_WeatherAPI_API_Key";
const char* location = "Oporto";  // "q" Parameter documentation for location: https://www.weatherapi.com/docs/#intro-request

// Request URL
String url = "https://api.weatherapi.com/v1/current.json?q=" + String(location) + "&key=" + String(api_key);

// TM1637 pins
#define CLK 19
#define DIO 18

TM1637Display display(CLK, DIO);

// Create the °C Symbol
const uint8_t Celsius[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
  SEG_A | SEG_D | SEG_E | SEG_F   // C
};

// Create the °F Symbol
const uint8_t Fahrenheit[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
  SEG_F | SEG_E | SEG_A | SEG_G   // F
};

// Global variables for temperatures and timers
float currentTempC = 9999;  // Initial error value
float currentTempF = 9999;  // Initial error value
unsigned long lastFetchTime = 0;
const unsigned long fetchInterval = 3600000UL;  // 1 hour in milliseconds

// Init Wifi
void initWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connection successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

bool updateTemperatures() {
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {  // Check for 200 specifically
    String payload = http.getString();
    Serial.print(payload);

    // Parse JSON
    StaticJsonDocument<1024> doc; // Adjust size if needed based on response
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      http.end();
      return false;
    }

    // Get both temperatures
    currentTempC = doc["current"]["temp_c"];
    currentTempF = doc["current"]["temp_f"];
    http.end();
    return true;
  } else {
    Serial.print("Error during request: ");
    Serial.println(httpCode);
    http.end();
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for Serial to initialize

  // Connect to Wi-Fi
  initWiFi();

  display.setBrightness(7);  // Set the display brightness (0-7)

  // Initial fetch
  if (updateTemperatures()) {
    lastFetchTime = millis();
  }
}

void loop() {
  // Check if it's time to fetch new data
  if (millis() - lastFetchTime >= fetchInterval) {
    if (updateTemperatures()) {
      lastFetchTime = millis();
    } else {
      // If failed, try again next loop 
      Serial.println("Fetch failed, keeping old values.");
    }
  }

  // Show Celsius
  Serial.print("Temperature in Celsius: ");
  Serial.println(currentTempC, 2);
  
  int roundedTempC = round(currentTempC);
  display.showNumberDec(roundedTempC, false, 2, 0);
  display.setSegments(Celsius, 2, 2);

  delay(5000);

  // Show Fahrenheit
  Serial.print("Temperature in Fahrenheit: ");
  Serial.println(currentTempF, 2);

  int roundedTempF = round(currentTempF);
  display.showNumberDec(roundedTempF, false, 2, 0);
  display.setSegments(Fahrenheit, 2, 2);

  delay(5000);
}
