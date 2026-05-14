/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp-idf-esp32-web-server-http-authentication/
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_http_server.h>
#include <esp_tls.h>
#include <esp_check.h>
#include <nvs_flash.h>
#include "esp_tls_crypto.h"
#include "sdkconfig.h"

#define MY_ESP_WIFI_SSID   CONFIG_ESP_WIFI_SSID
#define MY_ESP_WIFI_PASS   CONFIG_ESP_WIFI_PASSWORD

#define BASIC_AUTH_USER   "admin"
#define BASIC_AUTH_PASS   "password"

typedef struct {
    char    *username;
    char    *password;
} basic_auth_info_t;

#define HTTPD_401      "401 UNAUTHORIZED"    // HTTP Response 401

static const char *TAG = "web_server";

// HTML web page to serve the root /
static const char *root_page = 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>ESP-IDF: ESP32 Web Server</title>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<h1>ESP-IDF: ESP32 Web Server</h1>"
    "<p>Hello from ESP32!</p>"
    "<button onclick=\"window.location.href = '/logout';\">Logout</button>"
    "</body>"
    "</html>";

// HTML web page to serve the /logout (redirects the user to the root page after logout)
static const char *logout_page = 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>Logout</title>"
    "<script>"
    "function forceLogout(){"
    "  var xhr = new XMLHttpRequest();"
    "  xhr.open('GET', '/', true, 'logout', 'logout');"
    "  xhr.send();"
    "  setTimeout(function(){ window.location.href='/'; }, 500);"
    "}"
    "</script>"
    "</head>"
    "<body onload='forceLogout()'>"
    "<h2>Logging out...</h2>"
    "</body>"
    "</html>";

// Send 401 + WWW-Authenticate header
static esp_err_t send_401_response(httpd_req_t *req)
{
    httpd_resp_set_status(req, HTTPD_401);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"ESP32 Web Server\"");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// HTTP basic authentication base64 encoding function
static char *http_auth_basic(const char *username, const char *password)
{
    size_t out;
    char *user_info = NULL;
    char *digest = NULL;
    size_t n = 0;
    int rc = asprintf(&user_info, "%s:%s", username, password);
    if (rc < 0) {
        ESP_LOGE(TAG, "asprintf() returned: %d", rc);
        return NULL;
    }
    if (!user_info) {
        ESP_LOGE(TAG, "No enough memory for user information");
        return NULL;
    }
    esp_crypto_base64_encode(NULL, 0, &n, (const unsigned char *)user_info, strlen(user_info));
    // 6: The length of the "Basic " string
    // n: Number of bytes for a base64 encode format
    // 1: Number of bytes for a reserved which be used to fill zero
    digest = calloc(1, 6 + n + 1);
    if (digest) {
        strcpy(digest, "Basic ");
        esp_crypto_base64_encode((unsigned char *)digest + 6, n, &out, (const unsigned char *)user_info, strlen(user_info));
    }
    free(user_info);
    return digest;
}

// Check basic authentication
static bool check_basic_auth(httpd_req_t *req, const char *username, const char *password)
{
    char *buf = NULL;
    size_t buf_len = httpd_req_get_hdr_value_len(req, "Authorization") + 1;

    if (buf_len <= 1) {
        ESP_LOGE(TAG, "No Authorization header");
        return false;
    }

    buf = calloc(1, buf_len);
    if (!buf) {
        ESP_LOGE(TAG, "No memory for auth buffer");
        return false;
    }

    if (httpd_req_get_hdr_value_str(req, "Authorization", buf, buf_len) != ESP_OK) {
        free(buf);
        return false;
    }

    char *expected = http_auth_basic(username, password);
    if (!expected) {
        free(buf);
        return false;
    }

    bool authenticated = (strcmp(expected, buf) == 0);
    free(expected);
    free(buf);

    if (authenticated) {
        ESP_LOGI(TAG, "Authenticated user: %s", username);
    } else {
        ESP_LOGE(TAG, "Authentication failed");
    }
    return authenticated;
}

// GET handler for the /logout
static esp_err_t logout_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, logout_page, HTTPD_RESP_USE_STRLEN);

    ESP_LOGI(TAG, "Logout page served");
    return ESP_OK;
}

// URI registration structure for the /logout
static const httpd_uri_t logout_uri = {
    .uri       = "/logout",
    .method    = HTTP_GET,
    .handler   = logout_get_handler,
    .user_ctx  = NULL
};

// Root web page GET handler with basic authentication
static esp_err_t basic_auth_get_handler(httpd_req_t *req)
{
    basic_auth_info_t *basic_auth_info = req->user_ctx;
    
    if (!check_basic_auth(req, basic_auth_info->username, basic_auth_info->password)) {
        return send_401_response(req);
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, root_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// URI registration structure for the root / with basic authentication
static httpd_uri_t basic_auth = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = basic_auth_get_handler,
};

// Register the basic authentication URI handler
static void httpd_register_basic_auth(httpd_handle_t server)
{
    basic_auth_info_t *basic_auth_info = calloc(1, sizeof(basic_auth_info_t));
    if (basic_auth_info) {
        basic_auth_info->username = BASIC_AUTH_USER;
        basic_auth_info->password = BASIC_AUTH_PASS;

        basic_auth.user_ctx = basic_auth_info;
        httpd_register_uri_handler(server, &basic_auth);
    }
}

// Start the HTTP server
static httpd_handle_t start_web_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "HTTP server started on port %d", config.server_port);

        httpd_register_uri_handler(server, &logout_uri);
        httpd_register_basic_auth(server);

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
