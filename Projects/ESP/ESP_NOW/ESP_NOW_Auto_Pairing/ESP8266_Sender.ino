/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-auto-pairing-esp32-esp8266/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based on JC Servaye example: https://https://github.com/Servayejc/esp8266_espnow
*/
#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t channel = 1;
int readingId = 0;
int id = 2;

#define MAX_CHANNEL 13  // 11 in North America or 13 in Europe

unsigned long currentMillis = millis(); 
unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

uint8_t broadcastAddressX[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t clientMacAddress[6];

enum PairingStatus {PAIR_REQUEST, PAIR_REQUESTED, PAIR_PAIRED, };
PairingStatus pairingStatus = PAIR_REQUEST;

enum MessageType {PAIRING, DATA,};
MessageType messageType;

// Define variables to store DHT readings to be sent
float temperature;
float humidity;

// Define variables to store incoming readings
float incomingTemp;
float incomingHum;
int incomingReadingsId;

// Updates DHT readings every 10 seconds
//const long interval = 10000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  uint8_t msgType;
  uint8_t id;
  float temp;
  float hum;
  unsigned int readingId;
} struct_message;

typedef struct struct_pairing {       // new structure for pairing
    uint8_t msgType;
    uint8_t id;
    uint8_t macAddr[6];
    uint8_t channel;
} struct_pairing;

// Create a struct_message called myData
struct_message myData;
struct_message incomingReadings;
struct_pairing pairingData;

#define BOARD_ID 2
unsigned long start;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

void printIncomingReadings(){
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Temperature: ");
  Serial.print(incomingTemp);
  Serial.println(" ºC");
  Serial.print("Humidity: ");
  Serial.print(incomingHum);
  Serial.println(" %");
  Serial.print("Led: ");
  Serial.print(incomingReadingsId);
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  Serial.print("Size of message : ");
  Serial.print(len);
  Serial.print(" from ");
  printMAC(mac);
  Serial.println();
  uint8_t type = incomingData[0];
  switch (type) {
  case DATA :  
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    Serial.print(len);
    Serial.print(" Data bytes received from: ");
    printMAC(mac);
    Serial.println();
    incomingTemp = incomingReadings.temp;
    incomingHum = incomingReadings.hum;
    printIncomingReadings();
    
    if (incomingReadings.readingId % 2 == 1){
      digitalWrite(LED_BUILTIN, LOW);
    } else { 
      digitalWrite(LED_BUILTIN, HIGH);
    }
    break;

  case PAIRING:
    memcpy(&pairingData, incomingData, sizeof(pairingData));
    if (pairingData.id == 0) {                // the message comes from server
      Serial.print("Pairing done for ");
      printMAC(pairingData.macAddr);
      Serial.print(" on channel " );
      Serial.print(pairingData.channel);    // channel used by the server
      Serial.print(" in ");
      Serial.print(millis()-start);
      Serial.println("ms");
      //esp_now_del_peer(pairingData.macAddr);
      //esp_now_del_peer(mac);
      esp_now_add_peer(pairingData.macAddr, ESP_NOW_ROLE_COMBO, pairingData.channel, NULL, 0); // add the server to the peer list 
      pairingStatus = PAIR_PAIRED ;            // set the pairing status
    }
    break;
  }  
}

void getReadings(){
  // Read Temperature
  temperature = 22.5;
  humidity = 55.5;
}

void readGetMacAddress(){
  String val = WiFi.macAddress();
  Serial.println(val);
  char* endPtr; 
  clientMacAddress[0] = strtol(val.c_str(), &endPtr, 16); // read the first starting at the beginning of the buffer. this initializes endPtr as a pointer to the ':' after the first number 
  for (int i = 1;  (*endPtr) && (i < 6); i++) {
    clientMacAddress[i] = strtol(endPtr + 1, &endPtr, 16); // using +1 for the pointer as we want to skip the ':'
  }

  for (int i = 0; i < 6; i++) {
    Serial.print(clientMacAddress[i], HEX);
    if (i != 5) Serial.print(F(":"));
  }
}

PairingStatus autoPairing(){
  switch(pairingStatus) {
  case PAIR_REQUEST:
    Serial.print("Pairing request on channel "  );
    Serial.println(channel);
  
    // clean esp now
    esp_now_deinit();
    WiFi.mode(WIFI_STA);
    // set WiFi channel   
    wifi_promiscuous_enable(1);
    wifi_set_channel(channel);
    wifi_promiscuous_enable(0);
    //WiFi.printDiag(Serial);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
    }
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    // set callback routines
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    // set pairing data to send to the server
    pairingData.id = BOARD_ID;     
    pairingData.channel = channel;
    pairingData.macAddr[0] = clientMacAddress[0];
    pairingData.macAddr[1] = clientMacAddress[1];
    pairingData.macAddr[2] = clientMacAddress[2];
    pairingData.macAddr[3] = clientMacAddress[3];
    pairingData.macAddr[4] = clientMacAddress[4];
    pairingData.macAddr[5] = clientMacAddress[5];
    previousMillis = millis();
    // add peer and send request
    Serial.println(esp_now_send(broadcastAddressX, (uint8_t *) &pairingData, sizeof(pairingData)));
    pairingStatus = PAIR_REQUESTED;
    break;

  case PAIR_REQUESTED:
    // time out to allow receiving response from server
    currentMillis = millis();
    if(currentMillis - previousMillis > 1000) {
      previousMillis = currentMillis;
      // time out expired,  try next channel
      channel ++;
      if (channel > MAX_CHANNEL) {
        channel = 0;
      }
      pairingStatus = PAIR_REQUEST; 
    }
    break;

  case PAIR_PAIRED:
    //Serial.println("Paired!");
    break;
  }
  return pairingStatus;
} 



void setup() {
  // Init Serial Monitor
  Serial.begin(74880);
  pinMode(LED_BUILTIN, OUTPUT);
  // Init DHT sensor
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  readGetMacAddress();
  //Serial.println(WiFi.macAddress());
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  pairingData.id = 2;
}
 
void loop() { 
  if (autoPairing() == PAIR_PAIRED) { 
    static unsigned long lastEventTime = millis();
    static const unsigned long EVENT_INTERVAL_MS = 10000;
    if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
      Serial.print(".");
      getReadings();

      //Set values to send
      myData.msgType = DATA;
      myData.id = 2;
      myData.temp = temperature;
      myData.hum = humidity;
      myData.readingId = readingId ++;
      
      // Send message via ESP-NOW to all peers 
      esp_now_send(pairingData.macAddr, (uint8_t *) &myData, sizeof(myData));
      lastEventTime = millis();
    }
  }
}
