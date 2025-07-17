/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/getting-started-esp32-c3-super-mini/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Assign output variable to GPIO pin
const int output = 8;
String outputState = "OFF";

// Create a web server object
WebServer server(80);

// Function to handle turning GPIO on
void handleGPIOOn() {
  outputState = "ON";
  digitalWrite(output, LOW);
  handleRoot();
}

// Function to handle turning GPIO off
void handleGPIOOff() {
  outputState = "OFF";
  digitalWrite(output, HIGH);
  handleRoot();
}

// Function to handle the root URL and show the current state
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>";
  html += "html { font-family: Helvetica; text-align: center; background: #f5f7fa; margin: 0; padding: 20px; }";
  html += "body { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1 { color: #333; font-size: 28px; margin-bottom: 20px; }";
  html += "p { color: #555; font-size: 18px; margin: 10px 0; }";
  html += ".button { background: #4CAF50; border: none; color: white; padding: 12px 24px; text-decoration: none; font-size: 20px; border-radius: 8px; cursor: pointer; transition: background 0.2s ease; display: inline-block; width: 120px; box-sizing: border-box; }";
  html += ".button:hover { background: #45a049; }";
  html += ".button2 { background: #555555; }";
  html += ".button2:hover { background: #666666; }";
  html += "</style></head>";
  html += "<body><h1>ESP32 Web Server</h1>";

  // Display GPIO controls
  html += "<p>GPIO - State " + outputState + "</p>";
  if (outputState == "ON") {
    html += "<p><a href=\"/off\"><button class=\"button button2\">OFF</button></a></p>";
  } else {
    html += "<p><a href=\"/on\"><button class=\"button\">ON</button></a></p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Initialize the output variable as output
  pinMode(output, OUTPUT);
  // Set the onboard LED to LOW (inverted logic)
  digitalWrite(output, HIGH);

  // Connect to Wi-Fi network
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

  // Set up the web server to handle different routes
  server.on("/", handleRoot);
  server.on("/on", handleGPIOOn);
  server.on("/off", handleGPIOOff);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}