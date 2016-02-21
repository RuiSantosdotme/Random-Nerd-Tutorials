/*
 Created by Rui Santos
 
 All the resources for this project:
 http://randomnerdtutorials.com/
 
 Based on some ESP8266 code examples 
*/

#include <ESP8266WiFi.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* host = "maker.ifttt.com";
const char* apiKey = "YOUR_IFTTT_API_KEY";

int pin = 2;
volatile int state = false;
volatile int flag = false;
const char* door_state = "closed";

unsigned long previousMillis = 0; 
const long interval = 3000;

void changeDoorStatus() {

    unsigned long currentMillis = millis();
 
    if(currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;   
    
        state = !state;
        if(state) {
            door_state = "opened";
        }
        else{
            door_state = "closed";
        }
        flag = true;
        Serial.println(state);
        Serial.println(door_state);
    }
    
}


void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("Preparing the Door Status Monitor project...");
    
    pinMode(pin, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(pin), changeDoorStatus, CHANGE);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  
}

void loop() {

      if(flag){
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
                       "value1=" + door_state + "\r\n");
          flag = false;
      }  
      delay(10);
}
