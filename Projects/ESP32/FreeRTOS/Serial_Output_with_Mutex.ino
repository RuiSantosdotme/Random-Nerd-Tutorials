/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-mutex-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define MUTEX_TIMEOUT 5000  // 5s timeout

SemaphoreHandle_t serialMutex = NULL;

void Task1(void *parameter) {
  for (;;) {
    if (xSemaphoreTake(serialMutex, MUTEX_TIMEOUT)) {
      Serial.println("Task1: Logging from Task 1");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      Serial.println("Task1: End of log from Task 1");
      xSemaphoreGive(serialMutex);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void Task2(void *parameter) {
  for (;;) {
    if (xSemaphoreTake(serialMutex, MUTEX_TIMEOUT)) {
      Serial.println("Task2: Logging from Task 2");
      vTaskDelay(800 / portTICK_PERIOD_MS);
      Serial.println("Task2: End of log from Task 2");
      xSemaphoreGive(serialMutex);
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting FreeRTOS");

  serialMutex = xSemaphoreCreateMutex();
  if (serialMutex == NULL) {
    Serial.println("Failed to create mutex!");
    while (1);
  }

  xTaskCreatePinnedToCore(
    Task1,                  // Task function
    "Task1",                // Task name
    3000,                   // Stack size
    NULL,                   // Task parameters
    1,                      // Priority
    NULL,                   // Task handle
    1                       // Core ID
  );

  xTaskCreatePinnedToCore(
    Task2,                  // Task function
    "Task2",                // Task name
    3000,                   // Stack size
    NULL,                   // Task parameters
    2,                      // Higher priority
    NULL,                   // Task handle
    1                       // Core ID
  );
}

void loop() {
  
}
