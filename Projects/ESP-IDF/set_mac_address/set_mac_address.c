/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-mac-address/
*/
#include <stdio.h>
#include "esp_mac.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "CUSTOM_MAC";

// REPLACE THE NEXT VARIABLE WITH YOUR CUSTOM MAC ADDRESS
uint8_t custom_mac[6] = {0x32, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5}; 

void app_main(void)
{  
    // Set custom MAC Address
    esp_err_t err = esp_base_mac_addr_set(custom_mac);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Custom MAC address set successfully!");
    } else {
        ESP_LOGE(TAG, "Failed to set custom MAC address: %s", esp_err_to_name(err));
    }

    uint8_t read_mac[6];
    // Read new custom Wi-Fi STA MAC Address
    err = esp_read_mac(read_mac, ESP_MAC_WIFI_STA);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "[NEW] Wi-Fi STA MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                 read_mac[0], read_mac[1], read_mac[2],
                 read_mac[3], read_mac[4], read_mac[5]);
    }
}
