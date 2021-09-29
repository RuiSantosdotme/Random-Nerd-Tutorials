/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/door-status-monitor-using-the-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <ESP8266WiFi.h>

// Set GPIOs for LED and reedswitch
const int reedSwitch = 4;
const int led = 2; //optional

// Detects whenever the door changed state
bool changeState = false;

// Holds reedswitch state (1=opened, 0=close)
bool state;
String doorState;

// Auxiliary variables (it will only detect changes that are 1500 milliseconds apart)
unsigned long previousMillis = 0; 
const long interval = 1500;

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
const char* host = "maker.ifttt.com";
const char* apiKey = "REPLACE_WITH_YOUR_IFTTT_API_KEY";

// Runs whenever the reedswitch changes state
ICACHE_RAM_ATTR void changeDoorStatus() {
  Serial.println("State changed");
  changeState = true;
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Read the current door state
  pinMode(reedSwitch, INPUT_PULLUP);
  state = digitalRead(reedSwitch);

  // Set LED state to match door state
  pinMode(led, OUTPUT);
  digitalWrite(led, state);
  
  // Set the reedswitch pin as interrupt, assign interrupt function and set CHANGE mode
  attachInterrupt(digitalPinToInterrupt(reedSwitch), changeDoorStatus, CHANGE);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

void loop() {
  if (changeState){
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      // If a state has occured, invert the current door state   
      state = !state;
      if(state) {
        doorState = "closed";
      }
      else{
        doorState = "open";
      }
      digitalWrite(led, state);
      changeState = false;
      Serial.println(state);
      Serial.println(doorState);
        
      //Send email
      Serial.print("connecting to ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
    
      String url = "/trigger/door_status/with/key/";
      url += apiKey;
          
      Serial.print("Requesting URL: ");
      Serial.println(url);
      client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" + 
                          "Content-Type: application/x-www-form-urlencoded\r\n" + 
                          "Content-Length: 13\r\n\r\n" +
                          "value1=" + doorState + "\r\n");
    }  
  }
}
