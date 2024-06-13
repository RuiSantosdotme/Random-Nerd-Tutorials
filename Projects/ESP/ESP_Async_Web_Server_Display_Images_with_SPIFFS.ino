/*********
  Rui Santos
  This code has been deprecated, you should use the new code available: 
  https://RandomNerdTutorials.com/display-images-esp32-esp8266-web-server/
*********/

#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include "FS.h"
#endif

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <h2>ESP Image Web Server</h2>
  <img src="sun">
  <img src="sun-cloud">
  <img src="cloud">
  <img src="rain">
  <img src="storm">
  <img src="snow">
</body>  
</html>)rawliteral";


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  
  server.on("/sun", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/sun.png", "image/png");
  });
  server.on("/sun-cloud", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/sun-cloud.png", "image/png");
  });
  server.on("/cloud", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/cloud.png", "image/png");
  });
  server.on("/rain", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/rain.png", "image/png");
  });
  server.on("/storm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/storm.png", "image/png");
  });
  server.on("/snow", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/snow.png", "image/png");
  });
  // Start server
  server.begin();
}
 
void loop(){
  
}
