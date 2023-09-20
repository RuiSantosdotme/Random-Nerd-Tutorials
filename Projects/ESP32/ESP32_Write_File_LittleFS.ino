// Project details: https://RandomNerdTutorials.com/esp32-write-data-littlefs-arduino/

#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

#define FORMAT_LITTLEFS_IF_FAILED true

int mydata;

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}


void setup() {
  Serial.begin(115200);
  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
      Serial.println("LittleFS Mount Failed");
      return;
   }
   else{
       Serial.println("Little FS Mounted Successfully");
   }
   writeFile(LittleFS, "/data.txt", "MY ESP32 DATA \r\n");
}

void loop() {
  mydata = random (0, 1000);
  appendFile(LittleFS, "/data.txt", (String(mydata)+ "\r\n").c_str()); //Append data to the file
  readFile(LittleFS, "/data.txt"); // Read the contents of the file
  delay(30000);
}
