/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-arduino-tasks/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define LED1_PIN 2
#define LED2_PIN 4

TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;

void Task1(void *parameter) {
  pinMode(LED1_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED1_PIN, HIGH);
    Serial.println("Task1: LED1 ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(LED1_PIN, LOW);
    Serial.println("Task1: LED1 OFF");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task2(void *parameter) {
  pinMode(LED2_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED2_PIN, HIGH);
    Serial.println("Task2: LED2 ON");
    vTaskDelay(333 / portTICK_PERIOD_MS);
    digitalWrite(LED2_PIN, LOW);
    Serial.println("Task2: LED2 OFF");
    vTaskDelay(333 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    Task1,             // Task function
    "Task1",           // Task name
    10000,             // Stack size (bytes)
    NULL,              // Parameters
    1,                 // Priority
    &Task1Handle,      // Task handle
    1                  // Core 1
  );

  xTaskCreatePinnedToCore(
    Task2,            // Task function
    "Task2",          // Task name
    10000,            // Stack size (bytes)
    NULL,             // Parameters     
    1,                // Priority
    &Task2Handle,     // Task handle
    1                 // Core 1
  );
}

void loop() {
  // Empty because FreeRTOS scheduler runs the task
}