/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-light-sleep-arduino/
*********/

#include "esp_sleep.h"
#include "driver/rtc_io.h"

int counter = 0;
const int ledPin = 2;                      // GPIO pin for onboard LED
const gpio_num_t buttonPin1 = GPIO_NUM_26; // RTC IO for pushbutton 1
const gpio_num_t buttonPin2 = GPIO_NUM_27; // RTC IO for pushbutton 2
#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  // Macro for individual GPIO bitmask

// Create a bitmask for GPIO 26 and GPIO 27
uint64_t bitmask = BUTTON_PIN_BITMASK(buttonPin1) | BUTTON_PIN_BITMASK(buttonPin2);

// Method to print the GPIO that triggered the wakeup
void print_GPIO_wake_up(){
  int GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  Serial.print("GPIO that triggered the wake up: GPIO ");
  Serial.println((log(GPIO_reason))/log(2), 0);
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin1, INPUT_PULLDOWN); // pull-down resistor
    pinMode(buttonPin2, INPUT_PULLDOWN); // pull-down resistor

    // Configure GPIO 26 and GPIO 27 as RTC IOs for EXT1 wake-up
    rtc_gpio_deinit(buttonPin1);
    rtc_gpio_deinit(buttonPin2);

    // Enable EXT1 wake-up source
    esp_err_t result = esp_sleep_enable_ext1_wakeup(bitmask, ESP_EXT1_WAKEUP_ANY_HIGH);

    if (result == ESP_OK) {
        Serial.println("EXT1 Wake-Up set successfully.");
    } else {
        Serial.println("Failed to set EXT1 Wake-Up as wake-up source.");
    }
}

void loop() {
    Serial.printf("Counter: %d\n", counter);
    counter++;

    digitalWrite(ledPin, HIGH); // LED on to indicate wake-up
    delay(2000);
    digitalWrite(ledPin, LOW);  // Turn off LED before going to sleep

    Serial.println("Going into light sleep mode");
    delay(500);
    esp_light_sleep_start();    // Enter light sleep

    // After wake-up, disable the hold function on the RTC GPIOs
    rtc_gpio_hold_dis(buttonPin1);
    rtc_gpio_hold_dis(buttonPin2);

    Serial.println("----------------------");
    Serial.println("Returning from light sleep");
    // Print the GPIO (button) that caused the wake-up
    print_GPIO_wake_up();
}