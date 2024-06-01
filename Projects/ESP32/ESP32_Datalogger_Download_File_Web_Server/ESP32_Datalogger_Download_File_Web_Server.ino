/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-datalogger-download-data-file/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "time.h"
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

// Variable to save current epoch time
unsigned long epochTime; 

// Variables to hold sensor readings
float temp;
float hum;
float pres;
String dataMessage;

// File name where readings will be saved
const char* dataPath = "/data.txt";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// BME280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)
Adafruit_BME280 bme;

// Init BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

// Init microSD card
void initSDCard(){
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

// Delete file
void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\r\n", path);
  if(fs.remove(path)){
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
   //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

// Function that initializes wi-fi
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
  initBME();
  initSDCard();
  configTime(0, 0, ntpServer);

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Epoch Time, Temperature, Humidity, Pressure \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

  // Handle the root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/index.html", "text/html");
  });

  // Handle the download button
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/data.txt", String(), true);
  });

  // Handle the View Data button
  server.on("/view-data", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/data.txt", "text/plain", false);
  });

  // Handle the delete button
  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request){
    deleteFile(SD, dataPath);
    request->send(200, "text/plain", "data.txt was deleted.");
  });

  // Uncomment the following line if you need to serve more static files like CSS and javascript or favicon
  //server.serveStatic("/", SD, "/");

  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    //Get epoch time
    epochTime = getTime();
    
    //Get sensor readings
    temp = bme.readTemperature();
    //temp = 1.8*bme.readTemperature() + 32;
    hum = bme.readHumidity();
    pres = bme.readPressure()/100.0F;

    //Concatenate all info separated by commas
    dataMessage = String(epochTime) + "," + String(temp) + "," + String(hum) + "," + String(pres)+ "\r\n";
    Serial.print("Saving data: ");
    Serial.println(dataMessage);

    //Append the data to file
    appendFile(SD, "/data.txt", dataMessage.c_str());

    lastTime = millis();
  }
}
