/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-i2c-scanner/
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_SDA_PIN    8        // Change to your SDA pin
#define I2C_SCL_PIN    9        // Change to your SCL pin
#define I2C_TIMEOUT_MS 50       // Timeout for probing each address (in milliseconds)

static const char *TAG = "I2C_SCANNER";

void i2c_scanner_task(void *pvParameters)
{
    // Configure and initialize the I2C master bus
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    while (1) {
        ESP_LOGI(TAG, "Starting I2C scan...");

        uint8_t found_addresses[128] = {0};  // To store found devices
        int devices_found = 0;

        // Optional: Print classic grid header
        printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");

        for (int addr = 0x00; addr <= 0x7F; addr++) {
            if (addr % 16 == 0) {
                printf("%02x: ", addr);
            }
            // Skip general call (0x00) and reserved addresses
            esp_err_t ret = i2c_master_probe(bus_handle, addr, I2C_TIMEOUT_MS);

            if (ret == ESP_OK) {
                printf("%02x ", addr);
                if (addr >= 0x08 && addr <= 0x77) {  // Find only count valid 7-bit addresses
                    found_addresses[devices_found++] = addr;
                }
            } else if (ret == ESP_ERR_NOT_FOUND) {
                printf("-- ");
            } else {
                printf("UU ");  // Timeout or other error
            }

            if ((addr + 1) % 16 == 0) {
                printf("\n");
            }
        }
        // Print summary
        printf("\nScan complete: %d device(s) found.\n", devices_found);
        for (int i = 0; i < devices_found; i++) {
            printf("%d. Device 0x%02x address\n", i + 1, found_addresses[i]);
        }
        if (devices_found == 0) {
            printf("\n");  // Extra newline for readability when none found
        }
        // Repeats scan every 10 seconds (optional)
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "I2C scanner starting on SDA=%d, SCL=%d", I2C_SDA_PIN, I2C_SCL_PIN);
    xTaskCreate(i2c_scanner_task, "i2c_scanner", 4096, NULL, 5, NULL);
}
