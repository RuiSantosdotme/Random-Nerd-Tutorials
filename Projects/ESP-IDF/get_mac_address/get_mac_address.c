/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-mac-address/
*/
#include <stdio.h>
#include "esp_mac.h"
#include "esp_log.h"

static const char *TAG = "MAC";

void app_main(void)
{
    uint8_t mac[6];
    esp_err_t ret;

    // Base MAC Address (factory default)
    ret = esp_read_mac(mac, ESP_MAC_EFUSE_FACTORY);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Base MAC Address (Factory): %02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    // Wi-Fi Station MAC Address
    ret = esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Wi-Fi STA MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    // Wi-Fi SoftAP MAC Address
    ret = esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Wi-Fi SoftAP MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    // Bluetooth MAC Address
    ret = esp_read_mac(mac, ESP_MAC_BT);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Bluetooth MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
}
