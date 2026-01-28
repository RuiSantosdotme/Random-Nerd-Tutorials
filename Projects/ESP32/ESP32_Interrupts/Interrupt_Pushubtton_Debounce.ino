/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-gpio-interrupts-arduino/
*********/
#include <Arduino.h>

// Global variables for the button
const uint8_t buttonPin = 18;
volatile uint32_t counter = 0;
volatile bool pressed = false;

// For debouncing the pushbutton
const unsigned long DEBOUNCE_DELAY = 50;  // in milliseconds
volatile unsigned long lastPressTime = 0;

// Interrupt Service Routine (ISR)
void ARDUINO_ISR_ATTR buttonISR() {
  unsigned long now = millis();
  if (now - lastPressTime > DEBOUNCE_DELAY) {
    counter++;
    pressed = true;
  }
  lastPressTime = now;
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(buttonPin, buttonISR, HIGH);
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
