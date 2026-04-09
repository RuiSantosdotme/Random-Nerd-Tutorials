/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-web-server/
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "sdkconfig.h"

#define MY_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define MY_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

static const char *TAG = "web_server";

// HTML web page to serve 
static const char *html_page = 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>ESP-IDF: ESP32 Web Server</title>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<h1>ESP-IDF: ESP32 Web Server</h1>"
    "<p>Hello from ESP32!</p>"
    "</body>"
    "</html>";

// (Another option) HTML web page to serve (can be defined as a raw string literal for better readability)
/*
static const char *html_page = R"raw(
<!DOCTYPE html>
<html>
<head>
    <title>ESP-IDF: ESP32 Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
    <h1>ESP-IDF: ESP32 Web Server</h1>
    <p>Hello from ESP32!</p>
</body>
</html>
)raw";*/

// HTTP GET handler for root "/"
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Start the HTTP server
static httpd_handle_t start_web_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "HTTP server started on port %d", config.server_port);
        
        // Register URI handler
        httpd_uri_t uri_get = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = root_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_get);
        
        return server;
    }
    
    ESP_LOGE(TAG, "Failed to start HTTP server");
    return NULL;
}

// Wi-Fi and IP event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wi-Fi STA started. Connecting to %s...", MY_ESP_WIFI_SSID);
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Wi-Fi disconnected. Retrying connection...");
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
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
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
                                                        &wifi_event_handler,
                                                        NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL, NULL));

    // Configure Wi-Fi STA
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = MY_ESP_WIFI_SSID,
            .password = MY_ESP_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Start the web server (it will be ready once IP is assigned)
    httpd_handle_t server = start_web_server();
    if (server) {
        ESP_LOGI(TAG, "Web Server initialized. Waiting for Wi-Fi connection...");
    }
}
