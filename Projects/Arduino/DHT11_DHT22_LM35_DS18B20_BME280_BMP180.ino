/*
 * Rui Santos
 * Complete Project Details https://RandomNerdTutorials.com
 */

#include "DHT.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SD.h> // for the SD card

const int DHT11sensorPin = 11;  
const int DHT22sensorPin = 12;  

DHT dht11(DHT11sensorPin, DHT11);
DHT dht22(DHT22sensorPin, DHT22);

float DHT11temperature;
float DHT22temperature;

const int DS18B20sensorPin = 14;

OneWire oneWire(DS18B20sensorPin);
DallasTemperature ds18b20(&oneWire);

float DS18B20temperature1;
float DS18B20temperature2;

const int BME_SCK = 6;
const int BME_MISO = 4;
const int BME_MOSI = 7;
const int BME_CS = 5;

Adafruit_BME280 bme280(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

Adafruit_BMP085 bmp180;
 
const int LM35sensorPin = A0; 
float LM35sensorValue;
float LM35voltageOut;
float LM35temperature;

const int chipSelectSDCard = 53; 
File myFile;

void setup() {
  Serial.begin(9600);

  dht11.begin();
  delay(2000);
  dht22.begin();

  ds18b20.begin();

  bme280.begin();

  bmp180.begin();
 
  pinMode(LM35sensorPin, INPUT);

  if(!SD.begin(chipSelectSDCard)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");
  
  myFile=SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("File opened ok");
    // print the headings for our data
    myFile.println("DHT11,DHT22,DS18B20-1,DS18B20-2,BME280,BMP180,LM35");
  }
  myFile.close();  
}

void loop() {

  /*-------------------------------------------------------*/
  //DHT11
  DHT11temperature = dht11.readTemperature();
  if (isnan(DHT11temperature)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }
  Serial.print("Temperature DHT11(ºC): ");
  Serial.println(DHT11temperature);
  
  /*-------------------------------------------------------*/
  //DHT22  
  DHT22temperature = dht22.readTemperature();
  if (isnan(DHT22temperature)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }
  Serial.print("Temperature DHT22(ºC): ");
  Serial.println(DHT22temperature);

/*-------------------------------------------------*/  
  //DS18B20
  ds18b20.requestTemperatures();  
  DS18B20temperature1 = ds18b20.getTempCByIndex(0);
  DS18B20temperature2 = ds18b20.getTempCByIndex(1);

  Serial.print("Temperature DS18B20-1(ºC): ");
  Serial.println(DS18B20temperature1);
  
  Serial.print("Temperature DS18B20-2(ºC): ");
  Serial.println(DS18B20temperature2);

/*-------------------------------------------------*/
//BME280

  Serial.print("Temperature BME280(ºC): ");
  Serial.println(bme280.readTemperature());

/*-------------------------------------------------*/
//BMP180
  Serial.print("Temperature BMP180(ºC): ");
  Serial.println(bmp180.readTemperature());

/*-------------------------------------------------*/ 
  //LM35 SENSOR
  LM35sensorValue = analogRead(LM35sensorPin);
  LM35voltageOut = (LM35sensorValue * 5000) / 1024;
  
  // calculate temperature for LM35 (LM35DZ)
  LM35temperature = LM35voltageOut / 10;

  Serial.print("Temperature LM35(ºC): ");
  Serial.println(LM35temperature);

  Serial.println("");

  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("File open with success");
    myFile.print(DHT11temperature);
    myFile.print(",");
    myFile.print(DHT22temperature);
    myFile.print(",");
    myFile.print(DS18B20temperature1);
    myFile.print(",");
    myFile.print(DS18B20temperature2);
    myFile.print(",");
    myFile.print(bme280.readTemperature());
    myFile.print(",");
    myFile.print(bmp180.readTemperature());
    myFile.print(",");
    myFile.print(LM35temperature);
    myFile.println(",");
  }
  myFile.close();
  
  delay(6000);
}
