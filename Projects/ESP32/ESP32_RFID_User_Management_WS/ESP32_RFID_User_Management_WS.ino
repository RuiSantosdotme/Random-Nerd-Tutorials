/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-rfid-user-management-web-server/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <Arduino.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <time.h>
#include <WiFi.h>

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(5);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

long timezone = 0;
byte daysavetime = 1;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "uid";
const char* PARAM_INPUT_2 = "role";
const char* PARAM_INPUT_3 = "delete";
const char* PARAM_INPUT_4 = "delete-user";

String inputMessage;
String inputParam;

const int ledPin = 22;
const int buzzerPin = 4;

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

  time_t t = file.getLastWrite();
  struct tm *tmstruct = localtime(&t);

  char bufferDate[50]; // Adjust buffer size as needed
  snprintf(bufferDate, sizeof(bufferDate), "%d-%02d-%02d", 
          (tmstruct->tm_year) + 1900, 
          (tmstruct->tm_mon) + 1, 
          tmstruct->tm_mday);
  char bufferTime[50]; // Adjust buffer size as needed
  snprintf(bufferTime, sizeof(bufferTime), "%02d:%02d:%02d", 
          tmstruct->tm_hour, 
          tmstruct->tm_min, 
          tmstruct->tm_sec);
          
  String lastWriteTime = bufferDate;
  String finalString = String(bufferDate) + "," + String(bufferTime) + "," + String(message) + "\n";
  Serial.println(lastWriteTime);
  if(file.print(finalString.c_str())) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

// Append data to the SD card
void appendUserFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  String finalString = String(message) + "\n";

  if(file.print(finalString.c_str())) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

String processor(const String& var){
  return String("HTTP GET request sent to your ESP on input field (" 
                + inputParam + ") with value: " + inputMessage +
                "<br><a href=\"/\"><button class=\"button button-home\">Return to Home Page</button></a>");
}

void deleteLineFromFile(const char* filename, int lineNumber) {
  File file = SD.open(filename);
  if (!file) {
    Serial.println("Failed to open file for reading.");
    return;
  }

  // Read all lines except the one to delete
  String lines = "";
  int currentLine = 0;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (currentLine != lineNumber) {
      lines += line + "\n";
    }
    currentLine++;
  }
  file.close();

  // Write back all lines except the deleted one
  file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing.");
    return;
  }

  file.print(lines);
  file.close();
  Serial.println("Line deleted successfully.");
}

String getRoleFromFile(const char* filename, String uid) {
  File file = SD.open(filename);
  if (!file) {
    Serial.println("Failed to open file for reading.");
    return "";
  }

  // Skip the header line
  file.readStringUntil('\n');
  
  // Read each line and check for UID
  while (file.available()) {
    String line = file.readStringUntil('\n');
    
    int commaIndex = line.indexOf(',');
    if (commaIndex > 0) {
      String fileUID = line.substring(0, commaIndex);
      String role = line.substring(commaIndex + 1);

      // Compare UID
      if (fileUID == uid) {
        file.close();
        role.trim();  // Remove any extra spaces or newline characters
        return role;
      }
    }
  }
  file.close();
  return "";  // Return empty string if UID not found
}

void initRFIDReader() {
  mfrc522.PCD_Init();    // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.
	Serial.println(F("Scan PICC to see UID"));
}

void initLittleFS() {
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
        return;
  }
}
void initWifi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Print ESP32 Local IP Address
  Serial.print("ESP IP Address: ");
  Serial.println(WiFi.localIP());
}

void initTime() {
  Serial.println("CInitializing Time");
  struct tm tmstruct;
  delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  Serial.printf(
    "Time and Date right now is : %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min,
    tmstruct.tm_sec
  );
}

void initSDCard() {
  // CS pin = 15
  if (!SD.begin(15)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // If the log.txt file doesn't exist, create a file on the SD card and write the header
  File file = SD.open("/log.txt");
  if(!file) {
    Serial.println("log.txt file doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/log.txt", "Date,Time,UID,Role\r\n");
  }
  else {
    Serial.println("log.txt file already exists");  
  }
  file.close();

  // If the users.txt file doesn't exist, create a file on the SD card and write the header
  file = SD.open("/users.txt");
  if(!file) {
    Serial.println("users.txt file doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/users.txt", "UID,Role\r\n");
  }
  else {
    Serial.println("users.txt file already exists");  
  }
  file.close();
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  
  initRFIDReader();
  initLittleFS();
  initWifi();
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  initTime();
  initSDCard();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/full-log.html");
  });
  // Route for root /add-user web page
  server.on("/add-user", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/add-user.html");
  });
  // Route for root /manage-users web page
  server.on("/manage-users", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/manage-users.html");
  });

  // Serve Static files
  server.serveStatic("/", LittleFS, "/");

  // Loads the log.txt file
  server.on("/view-log", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/log.txt", "text/plain", false);
  });
  // Loads the users.txt file
  server.on("/view-users", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/users.txt", "text/plain", false);
  });
  
  // Receive HTTP GET requests on <ESP_IP>/get?input=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 and input2 value on <ESP_IP>/get?input1=<inputMessage1>&input2=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = String(PARAM_INPUT_1);
      inputMessage += " " + request->getParam(PARAM_INPUT_2)->value();
      inputParam += " " + String(PARAM_INPUT_2);

      String finalMessageInput = String(request->getParam(PARAM_INPUT_1)->value()) + "," + String(request->getParam(PARAM_INPUT_2)->value());
      appendUserFile(SD, "/users.txt", finalMessageInput.c_str());
    }
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = String(PARAM_INPUT_3);
      if(request->getParam(PARAM_INPUT_3)->value()=="users") {
        deleteFile(SD, "/users.txt");
      }
      else if(request->getParam(PARAM_INPUT_3)->value()=="log") {
        deleteFile(SD, "/log.txt");
      }
    }
    else if (request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
      inputParam = String(PARAM_INPUT_4);
      deleteLineFromFile("/users.txt", inputMessage.toInt());
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    request->send(LittleFS, "/get.html", "text/html", false, processor);
  });

  // Start server
  server.begin();
}

void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards.
	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

  // Save the UID on a String variable
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0"; 
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("Card UID: ");
  Serial.println(uidString);

  String role = getRoleFromFile("/users.txt", uidString);
  if (role != "") {
    Serial.print("Role for UID: ");
    Serial.print(uidString);
    Serial.print(" is ");
    Serial.println(role);
  } else {
    role = "unknown";
    Serial.print("UID: ");
    Serial.print(uidString);
    Serial.println(" not found, set user role to unknown");
  }
  String sdMessage = uidString + "," + role;
  appendFile(SD, "/log.txt", sdMessage.c_str());

  digitalWrite(buzzerPin, HIGH);
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(buzzerPin, LOW);
  delay(2500);
  digitalWrite(ledPin, LOW);
}