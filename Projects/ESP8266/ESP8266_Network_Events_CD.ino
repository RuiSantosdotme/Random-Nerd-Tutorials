/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/solved-reconnect-esp8266-nodemcu-to-wifi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi sucessfully.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi, trying to connect...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
}

void setup() {
  Serial.begin(115200);

  //Register event handlers
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
   
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}

void loop() {
  //delay(1000);
}
