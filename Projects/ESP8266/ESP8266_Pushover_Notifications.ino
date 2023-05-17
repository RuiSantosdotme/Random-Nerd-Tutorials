/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-pushover-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

const char* apiToken = "API_TOKEN";
const char* userToken = "USER_TOKEN";

//Pushover API endpoint
const char* pushoverApiEndpoint = "https://api.pushover.net/1/messages.json";

//Pushover root certificate (valid from 11/10/2006 to 11/10/2031)
const char *PUSHOVER_ROOT_CA = "-----BEGIN CERTIFICATE-----\n"
                 "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
                 "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                 "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
                 "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                 "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                 "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
                 "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
                 "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
                 "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
                 "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
                 "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
                 "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
                 "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
                 "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
                 "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
                 "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
                 "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
                 "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
                 "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
                 "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
                 "-----END CERTIFICATE-----\n";

// Create a list of certificates with the server certificate
X509List cert(PUSHOVER_ROOT_CA);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));


  //Make HTTPS POST request to send notification
  if (WiFi.status() == WL_CONNECTED) {
    // Create a JSON object with notification details
    // Check the API parameters: https://pushover.net/api
    StaticJsonDocument<512> notification;
    notification["token"] = apiToken;
    notification["user"] = userToken;
    notification["message"] = "Hello from ESP8266";
    notification["title"] = "ESP8266 Notification";
    notification["url"] = "";
    notification["url_title"] = "";
    notification["html"] = "";
    notification["priority"] = "";
    notification["sound"] = "cosmic";
    notification["timestamp"] = "";

    // Serialize the JSON object to a string
    String jsonStringNotification;
    serializeJson(notification, jsonStringNotification);

    // Create a WiFiClientSecure object
    WiFiClientSecure client;
    // Set the certificate
    client.setTrustAnchors(&cert);

    // Create an HTTPClient object
    HTTPClient http;

    // Specify the target URL
    http.begin(client, pushoverApiEndpoint);

    // Add headers
    http.addHeader("Content-Type", "application/json");

    // Send the POST request with the JSON data
    int httpResponseCode = http.POST(jsonStringNotification);

    // Check the response
    if (httpResponseCode > 0) {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
      String response = http.getString();
      Serial.println("Response:");
      Serial.println(response);
    } else {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
    }

    // Close the connection
    http.end();
  }
}

void loop() {
  
}
