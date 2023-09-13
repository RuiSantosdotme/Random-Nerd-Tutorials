/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ds18b20-arduino/
  
  Example from the library microSD18B20.h: https://github.com/GyverLibs/microDS18B20/tree/main/examples
  Get the address of DS18B20 sensor (one sensor connected at a time)
*********/

#include <microDS18B20.h>

// We're using GPIO 22, change accordingly
MicroDS18B20 <22> sensor;  

// Array to store the sensor address
uint8_t address[8];       

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (sensor.readAddress(address)) {
    Serial.print('{');
    for (uint8_t i = 0; i < 8; i++) {
      Serial.print("0x");
      Serial.print(address[i], HEX); 
      if (i < 7) Serial.print(", ");
    }
    Serial.println('}');

  } else Serial.println("Not connected");
  delay(1000);
}
