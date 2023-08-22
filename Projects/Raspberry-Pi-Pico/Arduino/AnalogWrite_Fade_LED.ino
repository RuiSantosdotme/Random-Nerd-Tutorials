/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-pwm-analogwrite-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// Pin connected to the LED
const int ledPin = 20;

// Time interval between brightness steps (in milliseconds)
const int fadeInterval = 10;

// Maximum and minimum brightness values
const int maxBrightness = 255;
const int minBrightness = 0;

// Current brightness value
int brightness = minBrightness;

// Direction flag for fading (true = increasing, false = decreasing)
bool fadingDirection = true;

void setup() {
  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  // Adjust PWM properties if needed
  /*analogWriteFreq(5000);
  analogWriteRange(65535);
  analogWriteResolution(16);*/
}

void loop() {
  // Update the brightness value based on the fading direction
  if (fadingDirection) {
    for (int i = brightness; i <= maxBrightness; i++) {
      brightness = i;
      Serial.println(brightness);
      
      // Set the LED brightness
      analogWrite(ledPin, brightness);
      
      // Delay for the fade interval
      delay(fadeInterval);
    }
    
    fadingDirection = false;
  } else {
    for (int i = brightness; i >= minBrightness; i--) {
      brightness = i;
      Serial.println(brightness);
      
      // Set the LED brightness
      analogWrite(ledPin, brightness);
      
      // Delay for the fade interval
      delay(fadeInterval);
    }
    
    fadingDirection = true;
  }
}
