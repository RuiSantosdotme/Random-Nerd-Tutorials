/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-gpio-pwm-ledc/
*/
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include "sdkconfig.h"

#define LED_PIN_1 4
#define LED_PIN_2 5
#define LEDC_CHANNEL_0 LEDC_CHANNEL_0
#define LEDC_CHANNEL_1 LEDC_CHANNEL_1
#define LEDC_TIMER_0 LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_10_BIT  // 10-bit resolution (0-1023)
#define LEDC_FREQUENCY 1000              // 1 kHz for smooth fading

void app_main(void)
{
    // Configure LEDC timer (one timer for all channels)
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = LEDC_FREQUENCY,
    };
    ledc_timer_config(&ledc_timer);

    // Configure LEDC channels
    ledc_channel_config_t ledc_channel_0 = {
        .gpio_num = LED_PIN_1,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
    ledc_channel_config(&ledc_channel_0);

    ledc_channel_config_t ledc_channel_1 = {
        .gpio_num = LED_PIN_2,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
    ledc_channel_config(&ledc_channel_1);

    // Fading loop
    int duty1 = 0, duty2 = 0;
    int step1 = 10, step2 = 20;
    while (1) {
        // LED 1: slow fade
        duty1 += step1;
        if (duty1 > 1023) {
            duty1 = 1023;
            step1 = -step1;
        } else if (duty1 < 0) {
            duty1 = 0;
            step1 = -step1;
        }
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, duty1);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);

        // LED 2: fast fade
        duty2 += step2;
        if (duty2 > 1023) {
            duty2 = 1023;
            step2 = -step2;
        } else if (duty2 < 0) {
            duty2 = 0;
            step2 = -step2;
        }
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, duty2);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);

        printf("LED #1 duty cycle: %d, LED #2 duty cycle: %d\n", duty1, duty2);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
