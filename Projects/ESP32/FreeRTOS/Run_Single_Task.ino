/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-arduino-tasks/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define LED_PIN 2

// Declare task handle
TaskHandle_t BlinkTaskHandle = NULL;

void BlinkTask(void *parameter) {
  for (;;) { // Infinite loop
    digitalWrite(LED_PIN, HIGH);
    Serial.println("BlinkTask: LED ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 1000ms
    digitalWrite(LED_PIN, LOW);
    Serial.println("BlinkTask: LED OFF");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.print("BlinkTask running on core ");
    Serial.println(xPortGetCoreID());
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);

  xTaskCreatePinnedToCore(
    BlinkTask,         // Task function
    "BlinkTask",       // Task name
    10000,             // Stack size (bytes)
    NULL,              // Parameters
    1,                 // Priority
    &BlinkTaskHandle,  // Task handle
    1                  // Core 1
  );
}

void loop() {
  // Empty because FreeRTOS scheduler runs the task
}