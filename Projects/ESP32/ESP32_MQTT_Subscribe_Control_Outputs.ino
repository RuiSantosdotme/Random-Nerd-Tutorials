/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details: https://randomnerdtutorials.com/esp32-big-timer-node-red/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <Arduino.h>
#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

// BROKER
#define MQTT_HOST IPAddress(192, 168, 1, XXX) //MQTT BROKER IP ADDRESS
/*for example:
#define MQTT_HOST IPAddress(192, 168, 1, 106)*/
#define MQTT_PORT 1883
#define BROKER_USER "REPLACE_WITH_BROKER_USERNAME"
#define BROKER_PASS "REPLACE_WITH_BROKER_PASSWORD"

//MQTT Subscribe Topics
#define MQTT_SUB_DIGITAL "esp32/digital/#"

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  //Subscribe to topics
  // Subscribe to topic MQTT_SUB_DIGITAL when it connects to the broker
  uint16_t packetIdSub1 = mqttClient.subscribe(MQTT_SUB_DIGITAL, 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub1);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  // Do whatever you want when you receive a message
  // Save the message in a variable
  String receivedMessage;
  for (int i = 0; i < len; i++) {
    Serial.println((char)payload[i]);
    receivedMessage += (char)payload[i];
  }
  // Save topic in a String variable
  String receivedTopic = String(topic);  
  Serial.print("Received Topic: ");
  Serial.println(receivedTopic);

  // Check which GPIO we want to control
  int stringLen = receivedTopic.length();
  // Get the index of the last slash
  int lastSlash = receivedTopic.lastIndexOf("/");
  // Get the GPIO number (it's after the last slash "/")
  // esp/digital/GPIO
  String gpio = receivedTopic.substring(lastSlash+1, stringLen);
  Serial.print("DIGITAL GPIO: ");
  Serial.println(gpio);
  Serial.print("STATE: ");
  Serial.println(receivedMessage);

  // Check if it is DIGITAL
  if (receivedTopic.indexOf("digital") > 0) {
    //Set the specified GPIO as output
    pinMode(gpio.toInt(), OUTPUT);
    //Control the GPIO
    if (receivedMessage == "1"){
      digitalWrite(gpio.toInt(), HIGH);
    }
    else{
      digitalWrite(gpio.toInt(), LOW);
    }
  }
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(BROKER_USER, BROKER_PASS);

  connectToWifi();
}

void loop() {

}
