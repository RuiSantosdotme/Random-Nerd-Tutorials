/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-freertos-queues-inter-task-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

#define QUEUE_SIZE 5

Adafruit_BME280 bme;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

QueueHandle_t sensorQueue = NULL;

typedef struct {
  float temperature;
  float humidity;
  float pressure;
} SensorData;

void SensorTask(void *parameter) {
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  for (;;) {
    SensorData data;
    data.temperature = bme.readTemperature();  
    data.humidity = bme.readHumidity();        
    data.pressure = bme.readPressure() / 100.0F;  
    xQueueSend(sensorQueue, &data, portMAX_DELAY);
    Serial.print("SensorTask: Sent Temp=");
    Serial.print(data.temperature, 1);
    Serial.print("°C, Hum=");
    Serial.print(data.humidity, 1);
    Serial.print("%, Pres=");
    Serial.print(data.pressure, 1);
    Serial.println("hPa");
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // 2s
  }
}

void DisplayTask(void *parameter) {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED init failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);

  for (;;) {
    SensorData data;
    if (xQueueReceive(sensorQueue, &data, portMAX_DELAY)) {

      // Display on OLED
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Temperature: ");
      display.print(data.temperature, 1);
      display.print(" ");
      display.cp437(true);
      display.write(167);
      display.println("C");
      display.setCursor(0, 20);
      display.print("Humidity: ");
      display.print(data.humidity, 1);
      display.println(" %");
      display.setCursor(0, 40);
      display.print("Pressure: ");
      display.print(data.pressure, 1);
      display.println(" hPa");
      display.display();
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Starts I2C on the board default's  I2C pins
  Wire.begin();

  // Create queue
  sensorQueue = xQueueCreate(QUEUE_SIZE, sizeof(SensorData));
  if (sensorQueue == NULL) {
    Serial.println("Failed to create queue!");
    while (1);
  }

  // Create tasks
  xTaskCreatePinnedToCore(
    SensorTask,             // Task function
    "SensorTask",           // Task name
    4000,                   // Stack size (bytes)
    NULL,                   // Task parameters
    1,                      // Priority
    NULL,                   // Task handle
    1                       // Core ID
  );

  xTaskCreatePinnedToCore(
    DisplayTask,            // Task function
    "DisplayTask",          // Task name
    4000,                   // Stack size (bytes)
    NULL,                   // Task parameters
    1,                      // Priority
    NULL,                   // Task handle
    1                       // Core ID
  );
}

void loop() {
  // Empty
}
