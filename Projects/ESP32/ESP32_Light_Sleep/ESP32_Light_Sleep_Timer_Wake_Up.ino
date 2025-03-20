/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-light-sleep-arduino/
*********/

int counter = 0;
const int ledPin = 2;           // GPIO pin for onboard LED
uint64_t sleepTime = 10000000;  // Sleep duration in microseconds (10 seconds)

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);

    // Enable wake-up by timer
    esp_err_t result = esp_sleep_enable_timer_wakeup(sleepTime);

    if (result == ESP_OK) {
        Serial.println("Timer Wake-Up set successfully as wake-up source.");
    } else {
        Serial.println("Failed to set Timer Wake-Up as wake-up source.");
    }
}

void loop() {
    Serial.printf("Counter: %d\n", counter);
    counter++;

    digitalWrite(ledPin, HIGH);  // LED on to indicate wake-up
    delay(2000);
    digitalWrite(ledPin, LOW);   // Turn off LED before going to sleep

    Serial.println("Going into light sleep mode");
    delay(500);
    esp_light_sleep_start();     // Enter light sleep
    Serial.println("Returning from light sleep");
}
