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
    Serial.printf("Task1 Stack Free: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
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
    Serial.printf("Task2 Stack Free: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.printf("Starting FreeRTOS: Memory Usage\nInitial Free Heap: %u bytes\n", xPortGetFreeHeapSize());

  xTaskCreatePinnedToCore(
    Task1,
    "Task1",
    10000,
    NULL,
    1,
    &Task1Handle,
    1
  );

  xTaskCreatePinnedToCore(
    Task2,
    "Task2",
    10000,
    NULL,
    1,
    &Task2Handle,
    1
  );
}

void loop() {
  static uint32_t lastCheck = 0;
  if (millis() - lastCheck > 5000) {
    Serial.printf("Free Heap: %u bytes\n", xPortGetFreeHeapSize());
    lastCheck = millis();
  }
}