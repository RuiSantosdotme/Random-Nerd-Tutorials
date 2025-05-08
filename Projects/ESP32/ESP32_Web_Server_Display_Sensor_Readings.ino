/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp32-web-server-beginners-guide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <WebServer.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Create an instance of the WebServer on port 80
WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}";
  html += "table { border-collapse: collapse; width:60%; margin-left:auto; margin-right:auto; }";
  html += "th { padding: 10px; background-color: #0043af; color: white; }";
  html += "tr { border: 1px solid #ddd; padding: 10px; }";
  html += "tr:hover { background-color: #bcbcbc; }";
  html += "td { border: none; padding: 8px; }";
  html += ".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }</style></head>";
  html += "<body><h1>ESP32 with BME280</h1>";
  html += "<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>";
  html += "<tr><td>Temp. Celsius</td><td><span class=\"sensor\">";
  html += String(bme.readTemperature());
  html += " *C</span></td></tr>";
  html += "<tr><td>Temp. Fahrenheit</td><td><span class=\"sensor\">";
  html += String(1.8 * bme.readTemperature() + 32);
  html += " *F</span></td></tr>";
  html += "<tr><td>Pressure</td><td><span class=\"sensor\">";
  html += String(bme.readPressure() / 100.0F);
  html += " hPa</span></td></tr>";
  html += "<tr><td>Approx. Altitude</td><td><span class=\"sensor\">";
  html += String(bme.readAltitude(SEALEVELPRESSURE_HPA));
  html += " m</span></td></tr>";
  html += "<tr><td>Humidity</td><td><span class=\"sensor\">";
  html += String(bme.readHumidity());
  html += " %</span></td></tr></table></body></html>";

  // Send the response to the client
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Initialize the BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up the routes
  server.on("/", handleRoot);
  
  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();
}
