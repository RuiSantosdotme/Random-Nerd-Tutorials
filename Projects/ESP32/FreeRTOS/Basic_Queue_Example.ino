/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-queues-inter-task-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define POT_PIN 15 
#define LED_PIN 2   
#define PWM_FREQ 5000
#define PWM_RESOLUTION 12  // 12-bit (0–4095)
#define QUEUE_SIZE 5

// Create an handle for the queue
QueueHandle_t potQueue = NULL;

void potTask(void *parameter) {
  for (;;) {
    uint16_t potValue = analogRead(POT_PIN);  // Read 0–4095
    xQueueSend(potQueue, &potValue, portMAX_DELAY);  // Send to queue
    Serial.printf("potTask: Sent pot value %u\n", potValue);
    vTaskDelay(100 / portTICK_PERIOD_MS);  // 100ms
  }
}

void LEDBrightnessTask(void *parameter) {
  for (;;) {
    uint16_t potValue;
    if (xQueueReceive(potQueue, &potValue, portMAX_DELAY)) {
      uint16_t brightness = potValue;
      ledcWrite(LED_PIN, brightness);
      Serial.printf("LEDBrightnessTask: Set brightness %u\n", brightness);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup PWM for LED
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);

  // Create queue (5 items, each uint16_t)
  potQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint16_t));
  if (potQueue == NULL) {
    Serial.println("Failed to create queue!");
    while (1);
  }

  // Create tasks
  xTaskCreatePinnedToCore(
    potTask,
    "potTask",
    3000,  // Task stack
    NULL,
    1,
    NULL,
    1  // Core 1
  );

  xTaskCreatePinnedToCore(
    LEDBrightnessTask,
    "LEDBrightnessTask",
    3000,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  // Empty
}
