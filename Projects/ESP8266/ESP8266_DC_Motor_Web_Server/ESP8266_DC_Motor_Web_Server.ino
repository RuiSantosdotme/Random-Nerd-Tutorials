/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp8266-nodemcu-web-server-dc-motor-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// Motor A pins
int motor1Pin1 = 12; 
int motor1Pin2 = 14; 
int enable1Pin = 13; 

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else {
    Serial.println("LittleFS mounted successfully");
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void moveForward(){
  Serial.println("Moving Forward");
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
}

void moveBackward(){
  Serial.println("Moving Backward");
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
}

void stopMotor(){
  digitalWrite(enable1Pin, LOW);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
}

void setup() {
  Serial.begin(115200);

  // Set motor pins as outputs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  initWiFi();
  initFS();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");

  server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request) {
    moveForward();
    request->send(LittleFS, "/index.html", "text/html", false);
  }); 

  server.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request) {
    moveBackward();
    request->send(LittleFS, "/index.html", "text/html", false);
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    stopMotor();
    request->send(LittleFS, "/index.html", "text/html", false);
  });

  server.begin();
}

void loop() {
  
}
