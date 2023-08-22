/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-analog-inputs-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// Potentiometer is connected to GPIO 26 (Analog ADC0)
const int potPin = 26;

// variable for storing the potentiometer value
int potValue = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  delay(1000);
}

void loop() {
  // Reading potentiometer value
  potValue = analogRead(potPin);
  Serial.println(potValue);
  delay(500);
}
