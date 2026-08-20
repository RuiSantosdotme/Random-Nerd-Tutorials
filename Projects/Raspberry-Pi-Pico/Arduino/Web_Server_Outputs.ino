/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-web-server-outputs-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
// Import required libraries
#include <WiFi.h>
#include <RPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// LED to control
#define LED_PIN 2   

// Current LED state (to display on the web page)
String ledState = "OFF";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Pico Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
      font-family: Arial;
      display: inline-block;
      margin: 0px auto;
      text-align: center;
    }
    body {
      max-width: 600px;
      margin: 0 auto;
      padding: 20px;
    }
    h1 {
      color: #333;
    }
    h2 {
      color: #666;
    }
    form {
      margin: 10px 0;
    }
    .button {
      background-color: #4CAF50;
      border: none;
      color: white;
      padding: 10px 20px;
      text-decoration: none;
      font-size: 20px;
      margin: 2px;
      cursor: pointer;
      border-radius: 5px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.2);
      transition: background-color 0.3s, box-shadow 0.3s;
    }
    .button:hover {
      background-color: #45a049;
      box-shadow: 0 4px 8px rgba(0,0,0,0.3);
    }
    .button2 {
      background-color: #f44336;
    }
    .button2:hover {
      background-color: #da190b;
    }
    .state {
      font-size: 1.4em;
      margin: 20px 0;
      color: #444;
    }
  </style>
</head>
<body>
  <h1>Raspberry Pi Pico Web Server</h1>
  <h2>Led Control</h2>
  <p class="state">LED State: %STATE%</p>
  <form action="./lighton">
    <input type="submit" value="LED on" class="button" />
  </form>
  <form action="./lightoff">
    <input type="submit" value="LED off" class="button button2" />
  </form>
</body>
</html>
)rawliteral";

// Processor function
String processor(const String& var){
  if(var == "STATE"){
    return ledState;
  }
  return String();
}

void setup(){
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print Pico Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html, processor);
  });

  // Route to turn LED ON
  server.on("/lighton", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_PIN, HIGH);
    ledState = "ON";
    request->send(200, "text/html", index_html, processor);
  });

  // Route to turn LED OFF
  server.on("/lightoff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LED_PIN, LOW);
    ledState = "OFF";
    request->send(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {

}
