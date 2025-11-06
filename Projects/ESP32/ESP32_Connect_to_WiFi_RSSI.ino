/*********
  Created by Ron Brinkman
  Complete instructions at https://RandomNerdTutorials.com/esp32-save-credentials-separate-file/
*********/

#include <WiFi.h>
#include <MyLogin.h>

// Replace with your network credentials (STATION)
const char*   ssid     = STASSID;
const char*   password = STAPSK;

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

void setup() {
  Serial.begin(115200);
  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop() {
  // put your main code here, to run repeatedly:
}
