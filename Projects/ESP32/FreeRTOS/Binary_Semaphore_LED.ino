/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-semaphores-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define BUTTON_PIN 23
#define LED1_PIN 2   // Toggled LED
#define LED2_PIN 4   // Blinking LED

#define DEBOUNCE_DELAY 200

SemaphoreHandle_t buttonSemaphore = NULL;

volatile uint32_t lastInterruptTime = 0;

void IRAM_ATTR buttonISR() {
  uint32_t currentTime = millis();
  if (currentTime - lastInterruptTime > DEBOUNCE_DELAY) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(buttonSemaphore, &higherPriorityTaskWoken);
    lastInterruptTime = currentTime;
    if (higherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
}

void LEDToggleTask(void *parameter) {
  pinMode(LED1_PIN, OUTPUT);
  bool ledState = false;
  for (;;) {
    if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY)) {
      ledState = !ledState;
      digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
      Serial.print("LEDToggleTask: LED1 ");
      Serial.println(ledState ? "ON" : "OFF");
    }
  }
}

void LEDBlinkTask(void *parameter) {
  pinMode(LED2_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED2_PIN, HIGH);
    Serial.println("LEDBlinkTask: LED2 ON");
    vTaskDelay(250 / portTICK_PERIOD_MS);
    digitalWrite(LED2_PIN, LOW);
    Serial.println("LEDBlinkTask: LED2 OFF");
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  // Defining the button as an interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  buttonSemaphore = xSemaphoreCreateBinary();
  if (buttonSemaphore == NULL) {
    Serial.println("Failed to create semaphore!");
    while (1);
  }

  xTaskCreatePinnedToCore(
    LEDToggleTask,          // Task function
    "LEDToggleTask",        // Task name
    3000,                   // Stack size
    NULL,                   // Task parameters
    2,                      // Higher priority
    NULL,                   // Task handle
    1                       // Core ID
  );

  xTaskCreatePinnedToCore(
    LEDBlinkTask,           // Task function
    "LEDBlinkTask",         // Task name
    3000,                   // Stack size
    NULL,                   // Task parameters
    1,                      // Medium priority
    NULL,                   // Task handle
    1                       // Core ID
  );
}

void loop() {
  
}
