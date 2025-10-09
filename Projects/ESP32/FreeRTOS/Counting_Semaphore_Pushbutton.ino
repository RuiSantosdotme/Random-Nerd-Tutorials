/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-semaphores-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define BUTTON_PIN 23
#define LED1_PIN 2    // Blinking LED
#define LED2_PIN 4    // Fading LED

#define DEBOUNCE_DELAY 200 // debounce for the pushbutton in milliseconds

#define SEMAPHORE_MAX_COUNT 5

SemaphoreHandle_t buttonSemaphore = NULL;
volatile uint32_t lastInterruptTime = 0;

void IRAM_ATTR buttonISR() {
  uint32_t currentTime = millis();
  if (currentTime - lastInterruptTime > DEBOUNCE_DELAY) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    if (xSemaphoreGiveFromISR(buttonSemaphore, &higherPriorityTaskWoken) == pdTRUE) {
      Serial.println("buttonISR: Gave semaphore token");
    } else {
      Serial.println("buttonISR: Semaphore full");
    }
    lastInterruptTime = currentTime;
    if (higherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
}

void LEDBlinkTask(void *parameter) {
  pinMode(LED1_PIN, OUTPUT);
  for (;;) {
    
    // Get and print the current semaphore count
    UBaseType_t count = uxSemaphoreGetCount(buttonSemaphore);
    Serial.print("LEDBlinkTask: Current semaphore count = ");
    Serial.println(count);

    if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY)) {
      Serial.println("LEDBlinkTask: Blinking LED1 for button press");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(LED1_PIN, HIGH);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(LED1_PIN, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void LEDFadeTask(void *parameter) {
  pinMode(LED2_PIN, OUTPUT);
  for (;;) {
    // Fade up (0 to 255)
    for (int duty = 0; duty <= 255; duty += 5) {
      analogWrite(LED2_PIN, duty);
      if (duty % 50 == 0) {  // Print every 10th step
        Serial.print("LEDFadeTask: Fading LED2, duty=");
        Serial.println(duty);
      }
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    // Fade down (255 to 0)
    for (int duty = 255; duty >= 0; duty -= 5) {
      analogWrite(LED2_PIN, duty);
      if (duty % 50 == 0 || duty == 255) {
        Serial.print("LEDFadeTask: Fading LED2, duty=");
        Serial.println(duty);
      }
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }
}

void setup() {
  Serial.begin(115200);  // Higher baud rate
  delay(1000);
  Serial.println("Starting FreeRTOS: Counting Semaphore with Fading Task Fixed");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  buttonSemaphore = xSemaphoreCreateCounting(SEMAPHORE_MAX_COUNT, 0);
  if (buttonSemaphore == NULL) {
    Serial.println("Failed to create semaphore!");
    while (1);
  }

  xTaskCreatePinnedToCore(
    LEDBlinkTask,           // Task function
    "LEDBlinkTask",         // Task name
    3000,                   // Stack size
    NULL,                   // Task parameters
    2,                      // Higher priority
    NULL,                   // Task handle
    1                       // Core ID
  );

  xTaskCreatePinnedToCore(
    LEDFadeTask,            // Task function
    "LEDFadeTask",          // Task name
    3000,                   // Stack size
    NULL,                   // Task parameters
    1,                      // Lower priority
    NULL,                   // Task handle
    1                       // Core ID
  );
}
void loop() {}
