/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-software-timers-interrupts/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define LED_PIN_RED 2
#define LED_PIN_BLUE 4

TimerHandle_t blinkTimerRed = NULL;
TimerHandle_t blinkTimerBlue = NULL;

bool ledStateRed = false;
bool ledStateBlue = false;

void BlinkRedCallback(TimerHandle_t xTimer) {
  ledStateRed = !ledStateRed;
  
  if (ledStateRed) {
    digitalWrite(LED_PIN_RED, HIGH);
    Serial.print("Red LED is ");
    Serial.println("ON");
  } else {
    digitalWrite(LED_PIN_RED, LOW);
    Serial.print("Red LED is ");
    Serial.println("OFF");
  }
}

void BlinkBlueCallback(TimerHandle_t xTimer) {
  ledStateBlue = !ledStateBlue;
  
  if (ledStateBlue) {
    digitalWrite(LED_PIN_BLUE, HIGH);
    Serial.print("Blue LED is ");
    Serial.println("ON");
  } else {
    digitalWrite(LED_PIN_BLUE, LOW);
    Serial.print("Blue LED is ");
    Serial.println("OFF");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting FreeRTOS");
  Serial.println("Periodic Timers for Two LEDs");

  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);

  blinkTimerRed = xTimerCreate(
    "BlinkTimerRed",               // Timer name
    700 / portTICK_PERIOD_MS,      // 1s period
    pdTRUE,                        // Auto-reload (periodic timer)
    NULL,                          // Timer ID
    BlinkRedCallback               // Callback function
  );
  if (blinkTimerRed == NULL) {
    Serial.println("Failed to create blinkTimerRed timer!");
    while (1);
  }

  blinkTimerBlue = xTimerCreate(
    "BlinkTimerBlue",               // Timer name
    1100 / portTICK_PERIOD_MS,      // 1s period
    pdTRUE,                         // Auto-reload (periodic timer)
    NULL,                           // Timer ID
    BlinkBlueCallback               // Callback function
  );
  if (blinkTimerBlue == NULL) {
    Serial.println("Failed to create blinkTimerBlue timer!");
    while (1);
  }

  xTimerStart(blinkTimerRed, 0); // Start timer immediately
  xTimerStart(blinkTimerBlue, 0); // Start timer immediately
}

void loop() {
  // Empty
}
