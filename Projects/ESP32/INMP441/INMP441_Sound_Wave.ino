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

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("INMP441 Microphone");

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
  Serial.println("Reading microphone...");
}

void loop() {
  size_t bytes_read = 0;

  // Read microphone data
  i2s_read(I2S_PORT, sBuffer, sizeof(sBuffer), &bytes_read, portMAX_DELAY);

  int samples_read = bytes_read / sizeof(int32_t);

  // Print the samples
  for (int i = 0; i < samples_read; i++) {

    // INMP441 sends 24-bit audio left-aligned in a 32-bit I2S word
    int32_t sample = sBuffer[i] >> 8;

    Serial.println(sample);
  }
}
