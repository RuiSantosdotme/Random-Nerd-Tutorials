/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/  
*********/

#include <Arduino.h>
#include "LittleFS.h"
 
void setup() {
  Serial.begin(9600);
  
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  
  File file = LittleFS.open("/text.txt", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  Serial.println("File Content:");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}
 
void loop() {

}
