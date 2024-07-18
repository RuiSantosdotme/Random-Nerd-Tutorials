/*
  Adapted from WriteSingleField Example from ThingSpeak Library (Mathworks)
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-thingspeak-publish-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

const char* ssid = "REPLACE_WITH_YOUR_SSID";   // your network SSID (name) 
const char* password = "REPLACE_WITH_YOUR_PASSWORD";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = X;
const char * myWriteAPIKey = "XXXXXXXXXXXXXXXX";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Variable to hold temperature readings
float temperatureC;
float humidity;
float pressure;
//uncomment if you want to get temperature in Fahrenheit
//float temperatureF;
String myStatus = "";

// Create a sensor object
Adafruit_BME280 bme; //BME280 connect to ESP8266 I2C (GPIO 4 = SDA, GPIO 5 = SCL)

void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  initBME();
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

    // Get a new temperature reading
    temperatureC = bme.readTemperature();
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureC);
    humidity = bme.readHumidity();
    Serial.print("Humidity (%): ");
    Serial.println(humidity);
    pressure = bme.readPressure() / 100.0F;
    Serial.print("Pressure (hPa): ");
    Serial.println(pressure);
    
    //uncomment if you want to get temperature in Fahrenheit
    /*temperatureF = 1.8 * bme.readTemperature() + 32;
    Serial.print("Temperature (ºF): ");
    Serial.println(temperatureF);*/

    // set the fields with the values
    ThingSpeak.setField(1, temperatureC);
    //ThingSpeak.setField(1, temperatureF);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, pressure);

    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}
