/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-gpio-interrupts-arduino/
*********/
#include <Arduino.h>

// Global variables for the button
const uint8_t buttonPin = 18;
volatile int32_t counter = 0;
volatile bool pressed = false;

// Interrupt Service Routine (ISR)
void ARDUINO_ISR_ATTR buttonISR() {
  counter++;
  pressed = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(buttonPin, buttonISR, RISING);
  Serial.println("Press the button on GPIO 18.");
}

void loop() {
  if (pressed) {
    Serial.print("Button pressed ");
    Serial.print(counter);
    Serial.println(" times.");
    pressed = false;
  }
  delay(10);
}