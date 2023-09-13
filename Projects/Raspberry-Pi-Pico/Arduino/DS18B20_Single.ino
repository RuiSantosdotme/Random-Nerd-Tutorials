/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-ds18b20-arduino/
  
  Example adapted from the microDS18B20 library examples folder - microSD18B20 library: https://github.com/GyverLibs/microDS18B20
*********/

#include <microDS18B20.h>

// We're using GPIO 22, change accordingly
MicroDS18B20<22> sensor;

void setup() {
  Serial.begin(115200);
}

void loop() {
  //request temperature from sensor
  sensor.requestTemp();

  delay(1000);
  
  //print the temperature
  Serial.print("Temperature (ºC): ");
  Serial.println(sensor.getTemp());

  //Temperature in Fahrenheit
  //Serial.print("Temperature (ºF): ");
  //Serial.println(sensor.getTemp()*(9.0/5.0) + 32.0);
}
