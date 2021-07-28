/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/altimeter-datalogger-esp32-bmp388/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//Libraries for microSD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"

AsyncWebServer server(80);

// Replace with your network credentials
const char* ssid     = "ESP32";
const char* password = NULL;

//OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables for BMP388
float seaLevelPressure = 1013.25;
Adafruit_BMP3XX bmp;
float alt;
float temp;
float pres;
String dataMessage;

//Pushbutton
const int buttonPin = 4; 
int buttonState;              
int lastButtonState = LOW;    
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers   

//Timers for datalogging
unsigned long lastTimer = 0;
unsigned long timerDelay = 18000;

const char* PARAM_INPUT_1 = "seaLevelPressure";

// HTML web page to handle 1 input field
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Sea Level Pressure</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Sea Level Pressure: <input type="float" name="seaLevelPressure">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void initBMP(){
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void getReadings(){
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  temp = bmp.temperature;
  pres = bmp.pressure / 100.0;
  alt = bmp.readAltitude(seaLevelPressure);
}

void initDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE); 
}

void displayReadings(){
  display.clearDisplay();
  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(String(temp));
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
  // display altitude
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Altitude: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(String(alt));
  display.print(" m"); 
  display.display();
}

// Initialize SD card
void initSDCard(){
   if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
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

// Initialize WiFi
void initWiFi() {
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

void setup() {
  Serial.begin(115200);
  initBMP();
  initDisplay();
  initSDCard();
  initWiFi();
  pinMode(buttonPin, INPUT);

  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Pressure, Altitude, Temperature \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      seaLevelPressure = inputMessage.toFloat();
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.begin();

}
void loop() {
  int reading = digitalRead(buttonPin);
  display.clearDisplay();

  // Light up when the pushbutton is pressed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        getReadings();
        displayReadings();
        delay(5000);
        display.clearDisplay();
        display.display();
        lastDebounceTime = millis();
      }
    }
  }
  lastButtonState = reading;

  // Log data every timerDelay seconds
  if ((millis() - lastTimer) > timerDelay) {
    //Concatenate all info separated by commas
    getReadings();
    dataMessage = String(pres) + "," + String(alt) + "," + String(temp)+ "," + String(seaLevelPressure) + "\r\n";
    Serial.print(dataMessage);
    //Append the data to file
    appendFile(SD, "/data.txt", dataMessage.c_str());
    lastTimer = millis();
  }
}
