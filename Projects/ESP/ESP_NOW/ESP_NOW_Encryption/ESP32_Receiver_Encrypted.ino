/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-esp-now-encrypted-messages/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR MASTER MAC Address
uint8_t masterMacAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// PMK and LMK keys
static const char* PMK_KEY_STR = "REPLACE_WITH_PMK_KEY";
static const char* LMK_KEY_STR = "REPLACE_WITH_LMK_KEY";

// Structure example to send data
// Must match the sender structure
typedef struct struct_message {
  int counter; // must be unique for each sender board
  int x;
  int y;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Function to print MAC address on Serial Monitor
void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
}

// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
 
  Serial.print("Packet received from: ");
  printMAC(mac_addr);
  
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Packet number: ");
  Serial.println(myData.counter);
  Serial.print("X: ");
  Serial.println(myData.x);
  Serial.print("Y: ");
  Serial.println(myData.y);
}
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("There was an error initializing ESP-NOW");
    return;
  }
  
  // Set the PMK key
  esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
  
  // Register the master as peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, masterMacAddress, 6);
  peerInfo.channel = 0;
  // Setting the master device LMK key
  for (uint8_t i = 0; i < 16; i++) {
    peerInfo.lmk[i] = LMK_KEY_STR[i];
  }
  // Set encryption to true
  peerInfo.encrypt = true;
  
  // Add master as peer       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
void loop() {
  
}
