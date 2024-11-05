/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/arduino-anemometer-measure-wind-speed/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files. The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Softwar
*********/

// Constants (Change the following variables if needed)
const int anemometerPin = 0;  // analog pin 0 (A0)
const float minVoltage = 0.054;  // Voltage corresponding to 0 m/s
const float maxVoltage = 5;  // Voltage corresponding to 32.4 m/s (max speed)
const float maxWindSpeed = 32.4; // Maximum wind speed in m/s

// Conversion factors
const float mps_to_kmh = 3.6;   // 1 m/s = 3.6 km/h
const float mps_to_mph = 2.23694; // 1 m/s = 2.23694 mph

void setup() {
  Serial.begin(9600);  
}

void loop() {
  // Read analog value from anemometer (ADC value between 0-1023 on Arduino for 0-5V)
  int adcValue = analogRead(anemometerPin);
  
  // Convert ADC value to voltage (Arduino ADC range is 0-5.0V)
  float voltage = (adcValue / 1023.00) * 5.0;
  
  // Ensure the voltage is within the anemometer operating range
  if (voltage < minVoltage) {
    voltage = minVoltage;
  } else if (voltage > maxVoltage) {
    voltage = maxVoltage;
  }
  
  // Map the voltage to wind speed
  float windSpeed_mps = ((voltage - minVoltage) / (maxVoltage - minVoltage)) * maxWindSpeed;

  // Convert wind speed to km/h and mph
  float windSpeed_kmh = windSpeed_mps * mps_to_kmh;
  float windSpeed_mph = windSpeed_mps * mps_to_mph;

  // Print wind speed
  Serial.print("Wind Speed: ");
  Serial.print(windSpeed_mps);
  Serial.print(" m/s, ");
  Serial.print(windSpeed_kmh);
  Serial.print(" km/h, ");
  Serial.print(windSpeed_mph);
  Serial.println(" mph");
  
  delay(1000); 
}
