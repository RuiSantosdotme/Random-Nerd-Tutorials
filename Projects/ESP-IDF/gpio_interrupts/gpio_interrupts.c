/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-gpio-interrupts/
*/
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include "sdkconfig.h"

#define BUTTON_GPIO GPIO_NUM_4       // Pushbutton GPIO
#define DEBOUNCE_DELAY_US 200000ULL  // Debounce delay in microseconds (200 ms)

static volatile uint64_t last_isr_time = 0;
static volatile uint32_t counter = 0;
static QueueHandle_t button_queue;

// Interrupt Service Routine (ISR) for button press, placed in IRAM for low latency
static void IRAM_ATTR button_isr(void *arg) {
    uint64_t now = esp_timer_get_time(); // Get current time in microseconds
    // Check if debounce period has passed, then process the button press
    if (now - last_isr_time > DEBOUNCE_DELAY_US) {
        counter++;
        uint32_t cnt = counter;
        BaseType_t higher_priority_task_woken = pdFALSE;
        xQueueSendFromISR(button_queue, &cnt, &higher_priority_task_woken); // Send counter to queue from ISR
        last_isr_time = now;
        if (higher_priority_task_woken) {
            portYIELD_FROM_ISR();
        }
    }
}

void app_main(void) {
    printf("Press the button on GPIO %d.\n", BUTTON_GPIO);

    // Create a queue to hold up to 10 uint32_t items
    button_queue = xQueueCreate(10, sizeof(uint32_t));

    // Configure Button GPIO
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE, // Rising edge interrupt trigger
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);

    // Install GPIO ISR service
    gpio_install_isr_service(0);

    // Add ISR handler for button
    gpio_isr_handler_add(BUTTON_GPIO, button_isr, NULL);

    // Variable to receive counter from queue
    uint32_t button_counter;

    // Keep program running
    while (1) {
        // Wait indefinitely for an item in the queue
        if (xQueueReceive(button_queue, &button_counter, portMAX_DELAY)) {
            printf("Button pressed %lu times.\n", button_counter);
        }
    }
}
