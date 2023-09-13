/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ds18b20-arduino/
*********/

#include <microDS18B20.h>

#define SENS_AMOUNT 4

// Add the address of each sensor
const uint8_t addr[][8] PROGMEM = {
  {0x28, 0xFF, 0xA0, 0x11, 0x33, 0x17, 0x3, 0x96},
  {0x28, 0xFF, 0xB4, 0x6, 0x33, 0x17, 0x3, 0x4B},
  {0x28, 0xAE, 0x26, 0x45, 0x92, 0x1, 0x2, 0x4B},
  {0x28, 0xFF, 0x11, 0x15, 0x33, 0x17, 0x3, 0x1A}
};

// All sensors are connected to GPIO22, adjust accordingly
MicroDS18B20<22, DS_ADDR_MODE, SENS_AMOUNT, DS_PROGMEM> sensors;

float sensor1TempC, sensor2TempC, sensor3TempC, sensor4TempC;
float sensor1TempF, sensor2TempF, sensor3TempF, sensor4TempF;

void setup() {
  Serial.begin(115200);
  
  // set the addresses
  sensors.setAddress((uint8_t*)addr);
}

void loop() {
  delay (1000);

  // Get temperature from each sensor
  sensor1TempC =  sensors.getTemp(0);
  sensor2TempC =  sensors.getTemp(1);
  sensor3TempC =  sensors.getTemp(2);
  sensor4TempC =  sensors.getTemp(3);

  //Convert temperatures to Fahrenheit
  sensor1TempF = sensor1TempC *(9.0/5.0) + 32.0;
  sensor2TempF = sensor2TempC *(9.0/5.0) + 32.0;
  sensor3TempF = sensor3TempC *(9.0/5.0) + 32.0;
  sensor4TempF = sensor4TempC *(9.0/5.0) + 32.0;
  
  Serial.print("Sensor 1 (temp ºC): "); 
  Serial.print(sensor1TempC);
  Serial.print(" Sensor 1 (temp ºF): "); 
  Serial.println(sensor1TempF);

  Serial.print("Sensor 2 (temp ºC): "); 
  Serial.print(sensor2TempC);
  Serial.print(" Sensor 2 (temp ºF): "); 
  Serial.println(sensor2TempF);

  Serial.print("Sensor 3 (temp ºC): "); 
  Serial.print(sensor3TempC);
  Serial.print(" Sensor 3 (temp ºF): "); 
  Serial.println(sensor3TempF);

  Serial.print("Sensor 4 (temp ºC): "); 
  Serial.print(sensor4TempC);
  Serial.print(" Sensor 4 (temp ºF): "); 
  Serial.println(sensor4TempF);

  Serial.print("");
  Serial.println("-----------------------------------");

  // request new temperature from all sensors
  sensors.requestTempAll();
}
