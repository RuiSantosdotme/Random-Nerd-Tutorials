/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-gpio-outputs/
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"

// Define the GPIO pin for the LED (GPIO 2 is common for onboard LEDs)
#define LED_PIN 2

void app_main(void)
{
    // Configure GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),     // Select GPIO 2
        .mode = GPIO_MODE_OUTPUT,              // Set as output
        .pull_up_en = GPIO_PULLUP_DISABLE,     // Disable pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Disable pull-down
        .intr_type = GPIO_INTR_DISABLE         // Disable interrupts
    };
    gpio_config(&io_conf);

    // Blink loop
    while (1) {
        // Turn LED ON
        printf("LED ON\n");
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second

        // Turn LED OFF
        printf("LED OFF\n");
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
    }
}
