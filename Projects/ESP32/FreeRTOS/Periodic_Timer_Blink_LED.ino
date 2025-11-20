/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-software-timers-interrupts/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Arduino.h>

#define LED_PIN 2

TimerHandle_t blinkTimer = NULL;

bool ledState = false;

void BlinkCallback(TimerHandle_t xTimer) {
  ledState = !ledState;
  
  if (ledState) {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("LED is ");
    Serial.println("ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.print("LED is ");
    Serial.println("OFF");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting FreeRTOS");
  Serial.println("Periodic Timer for LED Blink");

  pinMode(LED_PIN, OUTPUT);

  blinkTimer = xTimerCreate(
    "BlinkTimer",                   // Timer name
    1000 / portTICK_PERIOD_MS,      // 1s period
    pdTRUE,                         // Auto-reload (periodic timer)
    NULL,                           // Timer ID
    BlinkCallback                   // Callback function
  );
  if (blinkTimer == NULL) {
    Serial.println("Failed to create timer!");
    while (1);
  }

  xTimerStart(blinkTimer, 0); // Start timer immediately
}

void loop() {
  // Empty
}
