/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-mdns-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <mdns.h>     // For low-level mDNS functions from ESP-IDF

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// mDNS name (customize this as needed)
const char* mdnsName = "myesp32";

// Create a web server object
WebServer server(80);

// Function to handle the root URL
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>";
  html += "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: white; color: #333; margin: 20px; }";
  html += "h1 { color: #4a5568; font-weight: 300; }";
  html += "</style></head>";
  html += "<body>";
  html += "<h1>Hello World</h1>";
  html += "<p>ESP32 mDNS Demo.</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

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

  // Set mDNS
  if (!MDNS.begin(mdnsName)) {
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
  }

  delay(5000);

  // Add service to MDNS
  MDNS.addService("_http", "_tcp", 80);

  // Retrieve and print the actual (resolved) hostname - checking if there are any conflicts in the network
  char actualHostname[MDNS_NAME_BUF_LEN];  // MDNS_NAME_BUF_LEN is 64 from mdns.h
  esp_err_t err = mdns_hostname_get(actualHostname);
  if (err == ESP_OK) {
    String actualName = String(actualHostname);
    Serial.println("mDNS responder started. Given name: " + String(mdnsName));
    Serial.println("Current hostname: http://" + actualName + ".local");
    if (actualName != mdnsName) {
      Serial.println("Hostname renamed.");
    }
  } else {
    Serial.println("Failed to retrieve current hostname (error: " + String(err) + ")");
  }

  // Set up the web server to handle route
  server.on("/", handleRoot);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}
