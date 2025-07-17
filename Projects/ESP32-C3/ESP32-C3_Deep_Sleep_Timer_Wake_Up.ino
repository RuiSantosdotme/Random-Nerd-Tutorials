/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/getting-started-esp32-c3-super-mini/
*********/

#define uS_TO_S_FACTOR 1000000ULL   // Microseconds to seconds
#define TIME_TO_SLEEP 60            // Deep sleep duration in seconds
#define LED_PIN 4                   // LED connected to GPIO4

void setup() {
  delay(100);
  pinMode(LED_PIN, OUTPUT);

  // Blink LED
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);

  // Set deep sleep timer
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Enter deep sleep
  esp_deep_sleep_start();
}

void loop() {}