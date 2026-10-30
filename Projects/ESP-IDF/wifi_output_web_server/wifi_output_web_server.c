/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details: https://RandomNerdTutorials.com/esp-idf-esp32-web-server-output/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define MY_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define MY_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

static const char *TAG = "web_server";

// Define the GPIO pin for the LED
#define LED_PIN GPIO_NUM_5

// HTML web page to serve the root / page
static const char *html_root_page = 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>ESP-IDF: Output Web Server</title>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<style>"
    "body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding: 40px; margin: 0; }"
    "h1 { color: #333; margin-bottom: 10px; }"
    "p { font-size: 20px; color: #666; }"
    ".status { font-weight: bold; margin: 20px 0; }"
    ".status-on { color: #28a745; }"
    ".status-off { color: #dc3545; }"
    ".button {"
    "    display: inline-block; padding: 15px 32px; margin: 15px;"
    "    font-size: 20px; font-weight: bold; cursor: pointer; border: none;"
    "    border-radius: 8px; color: white; box-shadow: 0 4px 8px rgba(0,0,0,0.1);"
    "}"
    ".on { background-color: #28a745; }"
    ".off { background-color: #dc3545; }"
    ".button:hover { opacity: 0.9; transform: translateY(-2px); }"
    ".button:active { transform: translateY(1px); }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>ESP-IDF: Output Web Server</h1>"
    "<p>GPIO 5 Status: <span id=\"status\" class=\"status %s\">%s</span></p>"
    "<button onclick=\"setOutput(1)\" class=\"button on\">TURN ON</button>"
    "<button onclick=\"setOutput(0)\" class=\"button off\">TURN OFF</button>"
    "<script>"
    "function updateStatus() {"
    "    fetch('/status?gpio=5')"
    "        .then(r => r.text())"
    "        .then(text => {"
    "            const state = parseInt(text.trim());"
    "            const el = document.getElementById('status');"
    "            if (state === 1) {"
    "                el.className = 'status status-on';"
    "                el.textContent = 'ON';"
    "            } else {"
    "                el.className = 'status status-off';"
    "                el.textContent = 'OFF';"
    "            }"
    "        })"
    "        .catch(() => {});"
    "}"
    "function setOutput(state) {"
    "    fetch(`/output?gpio=5&state=${state}`)"
    "        .then(() => updateStatus())"
    "        .catch(() => {});"
    "}"
    "setInterval(updateStatus, 10000);"
    "window.onload = updateStatus;"
    "</script>"
    "</body>"
    "</html>";

// HTTP GET handler for root "/" - send main HTML page
static esp_err_t root_get_handler(httpd_req_t *req)
{
    int gpio_state = gpio_get_level(LED_PIN);
    const char *status_class = gpio_state ? "status-on" : "status-off";
    const char *status_text  = gpio_state ? "ON" : "OFF";

    char response[2048];
    snprintf(response, sizeof(response), html_root_page, status_class, status_text);

    httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store, must-revalidate");
    httpd_resp_set_hdr(req, "Pragma", "no-cache");
    httpd_resp_set_hdr(req, "Expires", "0");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

// Return current status as plain text "1" or "0"
static esp_err_t status_get_handler(httpd_req_t *req)
{
    char buf[100];
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;

    int gpio = LED_PIN;

    if (buf_len > 1 && httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
        char param[16];
        if (httpd_query_key_value(buf, "gpio", param, sizeof(param)) == ESP_OK) {
            gpio = atoi(param);
        }
    }

    if (gpio < 0 || gpio >= 40) {
        gpio = LED_PIN;   // Fallback to default if invalid
    }

    int state = gpio_get_level(gpio);

    char response[4];
    snprintf(response, sizeof(response), "%d", state);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

// Handle TURN ON / TURN OFF commands to control GPIO
static esp_err_t output_get_handler(httpd_req_t *req)
{
    char buf[100];
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;

    int gpio = LED_PIN;
    int state = 0;

    if (buf_len > 1 && httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
        char param[16];
        if (httpd_query_key_value(buf, "gpio", param, sizeof(param)) == ESP_OK) {
            gpio = atoi(param);
        }
        if (httpd_query_key_value(buf, "state", param, sizeof(param)) == ESP_OK) {
            state = atoi(param);
        }

        if ((state == 0 || state == 1) && gpio >= 0 && gpio < 40) {
            gpio_set_level(gpio, state);
            ESP_LOGI(TAG, "GPIO %d set to %d via web server", gpio, state);
        }
    }

    // Send empty OK response (client will call updateStatus())
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

// Start the HTTP server
static httpd_handle_t start_web_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;
    httpd_handle_t server = NULL;
    
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "HTTP server started on port %d", config.server_port);

        httpd_uri_t root = { .uri = "/", .method = HTTP_GET, .handler = root_get_handler };
        httpd_register_uri_handler(server, &root);

        httpd_uri_t output = { .uri = "/output", .method = HTTP_GET, .handler = output_get_handler };
        httpd_register_uri_handler(server, &output);

        httpd_uri_t status = { .uri = "/status", .method = HTTP_GET, .handler = status_get_handler };
        httpd_register_uri_handler(server, &status);

        return server;
    }
    ESP_LOGE(TAG, "Failed to start server");
    return NULL;
}

// Wi-Fi and IP event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Web Server ready! Access at http://" IPSTR "/", IP2STR(&event->ip_info.ip));
    }
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize TCP/IP stack and event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default Wi-Fi STA interface
    esp_netif_create_default_wifi_sta();

    // Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, 
                                                        ESP_EVENT_ANY_ID, 
                                                        wifi_event_handler, 
                                                        NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, 
                                                        IP_EVENT_STA_GOT_IP, 
                                                        wifi_event_handler, 
                                                        NULL, NULL));
    // Configure Wi-Fi STA
    wifi_config_t wifi_config = { 
        .sta = { 
            .ssid = MY_ESP_WIFI_SSID, 
            .password = MY_ESP_WIFI_PASS 
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Configure GPIO 5
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL << LED_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    gpio_set_level(LED_PIN, 0);

    // Start the web server (it will be ready once IP is assigned)
    httpd_handle_t server = start_web_server();
    if (server) {
        ESP_LOGI(TAG, "Web Server initialized. Waiting for Wi-Fi connection...");
    }
}
