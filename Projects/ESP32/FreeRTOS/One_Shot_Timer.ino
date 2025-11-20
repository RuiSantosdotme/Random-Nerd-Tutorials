/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-software-timers-interrupts/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define BUTTON_PIN 4
#define LED_PIN 2
#define DEBOUNCE_DELAY 200
#define TIMER_DELAY 5000

TimerHandle_t toggleTimer = NULL;
volatile uint32_t lastInterruptTime = 0;
volatile bool ledState = false;

void IRAM_ATTR buttonISR() {
  uint32_t currentTime = millis();
  if (currentTime - lastInterruptTime > DEBOUNCE_DELAY) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xTimerStartFromISR(toggleTimer, &higherPriorityTaskWoken);
    Serial.print("buttonISR: LED will ");
    if (ledState) {
      Serial.print("turn OFF");
    } else {
      Serial.print("turn ON");
    }
    Serial.print(" in 5 seconds");
    Serial.println();
    lastInterruptTime = currentTime;
    if (higherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
}

void ToggleCallback(TimerHandle_t xTimer) {
  ledState = !ledState;
  if (ledState) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED is ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED is OFF");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting FreeRTOS");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  toggleTimer = xTimerCreate(
    "ToggleTimer",                    // Timer name
    TIMER_DELAY / portTICK_PERIOD_MS, // 5s delay
    pdFALSE,                          // One-shot
    NULL,                             // Timer ID
    ToggleCallback                   // Callback
  );
  if (toggleTimer == NULL) {
    Serial.println("Failed to create timer!");
    while (1);
  }
}

void loop() {
  // Empty
}
