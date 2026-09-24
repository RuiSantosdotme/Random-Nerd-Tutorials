/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-inmp441-i2s-microphone-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <driver/i2s.h>

// Pin configuration
#define I2S_SCK   42   // BCLK
#define I2S_WS    40   // LRCLK / WS
#define I2S_SD    41   // DOUT from mic

#define I2S_PORT  I2S_NUM_0
#define SAMPLE_RATE 16000
#define BUFFER_LEN  512

int32_t sBuffer[BUFFER_LEN];

// LED
#define LED_PIN 38

// Clap detection
#define CLAP_THRESHOLD 1000000

// Maximum time allowed between the first and second clap
const unsigned long CLAP_GAP = 700;

// Prevent the same clap from being detected multiple times
const unsigned long CLAP_COOLDOWN = 250;

// State variables
bool ledState = false;

int clapCount = 0;

unsigned long firstClapTime = 0;
unsigned long lastClapTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("INMP441 Microphone");

  // LED setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // I2S configuration
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,   // L/R = GND
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_LEN,
    .use_apll = false
  };

  // Pin configuration
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  // Start I2S
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);

  Serial.println("I2S started.");
  Serial.println("Double clap to control LED");
}


void loop() {
  size_t bytes_read = 0;

  // Read microphone data
  i2s_read(I2S_PORT, sBuffer, sizeof(sBuffer), &bytes_read, portMAX_DELAY);

  int samples_read = bytes_read / sizeof(int32_t);

  // Find the loudest sample
  int32_t peak = 0;

  // Print the samples
  for (int i = 0; i < samples_read; i++) {
    int32_t sample = sBuffer[i] >> 8;
    int32_t magnitude = abs(sample);

    if (magnitude > peak) {
      peak = magnitude;
    }
  }

  // Print peak
  Serial.println(peak);

  // Detect a clap
  unsigned long now = millis();

  if (peak > CLAP_THRESHOLD && now - lastClapTime > CLAP_COOLDOWN) {
    lastClapTime = now;

    // First clap
    if (clapCount == 0) {
      clapCount = 1;
      firstClapTime = now;
      Serial.println("First clap detected");
    }

    // Second clap
    else if (now - firstClapTime <= CLAP_GAP) {
      clapCount = 0;

      // Double clap - invert LED state
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      if (ledState) {
        Serial.println("DOUBLE CLAP - LED ON");
      }
      else {
        Serial.println("DOUBLE CLAP - LED OFF");
      }
    }

    // Too much time passed
    else {

      // Treat this clap as the beginning of a new pair
      clapCount = 1;
      firstClapTime = now;
      Serial.println("New first clap");
    }
  }

  // Reset if second clap doesn't arrive in time
  if (clapCount == 1 && now - firstClapTime > CLAP_GAP) {
    clapCount = 0;
  }
}
