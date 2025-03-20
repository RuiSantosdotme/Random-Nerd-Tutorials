/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-light-sleep-arduino/
*********/

#include "esp_sleep.h"
#include "driver/gpio.h"

int counter = 0;
const int ledPin = 2;                      // GPIO pin for onboard LED
const gpio_num_t buttonPin1 = GPIO_NUM_26; // GPIO for pushbutton 1
const gpio_num_t buttonPin2 = GPIO_NUM_27; // GPIO for pushbutton 2

int wakeup_gpio; // Variable to store the GPIO that caused wake-up

// ISR for buttonPin1
void IRAM_ATTR handleInterrupt1() {
    wakeup_gpio = buttonPin1;
}

// ISR for buttonPin2
void IRAM_ATTR handleInterrupt2() {
    wakeup_gpio = buttonPin2;
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin1, INPUT_PULLDOWN); // pull-down resistor
    pinMode(buttonPin2, INPUT_PULLDOWN); // pull-down resistor

    // Configure GPIOs as wake-up source
    gpio_wakeup_enable(buttonPin1, GPIO_INTR_HIGH_LEVEL); // Trigger wake-up on high level
    gpio_wakeup_enable(buttonPin2, GPIO_INTR_HIGH_LEVEL); // Trigger wake-up on high level

    // Enable GPIO wake-up source
    esp_err_t result = esp_sleep_enable_gpio_wakeup();

    if (result == ESP_OK) {
        Serial.println("GPIO Wake-Up set successfully.");
    } else {
        Serial.println("Failed to set GPIO Wake-Up as wake-up source.");
    }

    // Attach interrupts to GPIO pins
    attachInterrupt(digitalPinToInterrupt(buttonPin1), handleInterrupt1, RISING);
    attachInterrupt(digitalPinToInterrupt(buttonPin2), handleInterrupt2, RISING);
}

void loop() {
    Serial.printf("Counter: %d\n", counter);
    counter++;

    digitalWrite(ledPin, HIGH); // LED on to indicate wake-up
    delay(5000);
    digitalWrite(ledPin, LOW);  // Turn off LED before going to sleep

    Serial.println("Going into light sleep mode");
    delay(500);
    esp_light_sleep_start();    // Enter light sleep

    Serial.println("----------------------");
    Serial.println("Returning from light sleep");

    // Print the GPIO that caused the wake-up
    Serial.printf("Wake-up caused by GPIO %d\n", wakeup_gpio);
}