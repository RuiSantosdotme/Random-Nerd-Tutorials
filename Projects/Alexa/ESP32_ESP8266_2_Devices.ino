/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
*/

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #define RF_RECEIVER 13
  #define RELAY_PIN_1 12
  #define RELAY_PIN_2 14
#else
  #include <ESP8266WiFi.h>
  #define RF_RECEIVER 5
  #define RELAY_PIN_1 4
  #define RELAY_PIN_2 14
#endif
#include "fauxmoESP.h"

#include <RCSwitch.h>

#define SERIAL_BAUDRATE 115200

#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASS "REPLACE_WITH_YOUR_PASSWORD"

#define LAMP_1 "lamp one"
#define LAMP_2 "lamp two"

fauxmoESP fauxmo;

RCSwitch mySwitch = RCSwitch();

// Wi-Fi Connection
void wifiSetup() {
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void setup() {
  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();

  // Wi-Fi connection
  wifiSetup();

  // LED
  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, HIGH);

  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_2, HIGH);
  
  mySwitch.enableReceive(RF_RECEIVER);  // Receiver on interrupt 0 => that is pin #2

  // By default, fauxmoESP creates it's own webserver on the defined port
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);
  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn lamp two on"

  // Add virtual devices
  fauxmo.addDevice(LAMP_1);
  fauxmo.addDevice(LAMP_2);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    // Callback when a command from Alexa is received. 
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.
        
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
    if ( (strcmp(device_name, LAMP_1) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("RELAY 1 switched by Alexa");
      //digitalWrite(RELAY_PIN_1, !digitalRead(RELAY_PIN_1));
      if (state) {
        digitalWrite(RELAY_PIN_1, LOW);
      } else {
        digitalWrite(RELAY_PIN_1, HIGH);
      }
    }
    if ( (strcmp(device_name, LAMP_2) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("RELAY 2 switched by Alexa");
      if (state) {
        digitalWrite(RELAY_PIN_2, LOW);
      } else {
        digitalWrite(RELAY_PIN_2, HIGH);
      }
    }
  });

}

void loop() {
  // fauxmoESP uses an async TCP server but a sync UDP server
  // Therefore, we have to manually poll for UDP packets
  fauxmo.handle();

  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }
    
  if (mySwitch.available()) {    
  /*Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );*/
    if (mySwitch.getReceivedValue()==6819768) {
      digitalWrite(RELAY_PIN_1, !digitalRead(RELAY_PIN_1));
    }
    if (mySwitch.getReceivedValue()==9463928) {
      digitalWrite(RELAY_PIN_2, !digitalRead(RELAY_PIN_2));     
    }
    delay(600);
    mySwitch.resetAvailable();
  }
}
