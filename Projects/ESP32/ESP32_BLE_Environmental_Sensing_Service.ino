/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-ble-server-environmental-sensing-service/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files. 
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//BLE server name
#define bleServerName "ESP32_BME280"

// Default UUID for Environmental Sensing Service
// https://www.bluetooth.com/specifications/assigned-numbers/
#define SERVICE_UUID (BLEUUID((uint16_t)0x181A))

// Temperature Characteristic and Descriptor (default UUID)
// Check the default UUIDs here: https://www.bluetooth.com/specifications/assigned-numbers/
BLECharacteristic temperatureCharacteristic(BLEUUID((uint16_t)0x2A6E), BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor temperatureDescriptor(BLEUUID((uint16_t)0x2902));

// Humidity Characteristic and Descriptor (default UUID)
BLECharacteristic humidityCharacteristic(BLEUUID((uint16_t)0x2A6F), BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor humidityDescriptor(BLEUUID((uint16_t)0x2902));

// Pressure Characteristic and Descriptor (default UUID)
BLECharacteristic pressureCharacteristic(BLEUUID((uint16_t)0x2A6D), BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor pressureDescriptor(BLEUUID((uint16_t)0x2902));

// Create a sensor object
Adafruit_BME280 bme;

// Init BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

bool deviceConnected = false;

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device Connected");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device Disconnected");
  }
};

void setup() {
  // Start serial communication 
  Serial.begin(115200);

  // Start BME sensor
  initBME();

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *bmeService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and corresponding Descriptors
  bmeService->addCharacteristic(&temperatureCharacteristic);
  temperatureCharacteristic.addDescriptor(&temperatureDescriptor);
  
  bmeService->addCharacteristic(&humidityCharacteristic);
  humidityCharacteristic.addDescriptor(&humidityDescriptor);

  bmeService->addCharacteristic(&pressureCharacteristic);
  pressureCharacteristic.addDescriptor(&pressureDescriptor);
  
  // Start the service
  bmeService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    // Read temperature as Celsius (the default)
    float t = bme.readTemperature();
    // Read humidity
    float h = bme.readHumidity();
    // Read pressure
    float p = bme.readPressure()/100.0F;
    
    //Notify temperature reading
    uint16_t temperature = (uint16_t)t;
    //Set temperature Characteristic value and notify connected client
    temperatureCharacteristic.setValue(temperature);
    temperatureCharacteristic.notify();
    Serial.print("Temperature Celsius: ");
    Serial.print(t);
    Serial.println(" ºC");
   
    //Notify humidity reading
    uint16_t humidity = (uint16_t)h;
    //Set humidity Characteristic value and notify connected client
    humidityCharacteristic.setValue(humidity);
    humidityCharacteristic.notify();   
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println(" %");

    //Notify pressure reading
    uint16_t pressure = (uint16_t)p;
    //Set humidity Characteristic value and notify connected client
    pressureCharacteristic.setValue(pressure);
    pressureCharacteristic.notify();   
    Serial.print("Pressure: ");
    Serial.print(p);
    Serial.println(" hPa");
    
    delay(10000);
  }
}
