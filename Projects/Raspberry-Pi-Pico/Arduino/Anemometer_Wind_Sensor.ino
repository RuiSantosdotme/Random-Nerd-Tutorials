/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-anemometer-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// Constants (Change the following variables if needed)
const int anemometerPin = 26;  // GPIO pin connected to anemometer (analog pin)
const float minVoltage = 0.033;  // Voltage corresponding to 0 m/s
const float maxVoltage = 3.3;  // Voltage corresponding to 32.4 m/s (max speed) (when using voltage divider)
const float maxWindSpeed = 32.4; // Maximum wind speed in m/s

// Conversion factors
const float mps_to_kmh = 3.6;   // 1 m/s = 3.6 km/h
const float mps_to_mph = 2.23694; // 1 m/s = 2.23694 mph

void setup() {
  Serial.begin(115200);  
}

void loop() {
  // Read analog value from anemometer (ADC value between 0-4095 on RPi Pico for 0-3.3V)
  int adcValue = analogRead(anemometerPin);
  Serial.print(adcValue);
  
  // Convert ADC value to voltage (by default, code sets 10-bit resolution: 0-1023 ADC range is 0-3.3V)
  float voltage = (adcValue / 1023.00) * 3.3;
  
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
