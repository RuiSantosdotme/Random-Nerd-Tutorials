/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/send-sms-esp32-twilio/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// Library example: https://github.com/ademuri/twilio-esp32-client

#include "twilio.hpp"

// Set these - but DON'T push them to GitHub!
static const char *ssid = "REPLACE_WITH_YOUR_SSID";
static const char *password = "REPLACE_WITH_YOUR_PASSWORD";

// Values from Twilio (find them on the dashboard)
static const char *account_sid = "REPLACE_WITH_YOUR_ACCOUNT_SID";
static const char *auth_token = "REPLACE_WITH_YOUR_ACCOUNT_AUTH_TOKEN";
// Phone number should start with "+<countrycode>"
static const char *from_number = "REPLACE_WITH_TWILIO_NUMBER";

// You choose!
// Phone number should start with "+<countrycode>"
static const char *to_number = "REPLACE_WITH_RECIPIENT_NUMBER";
static const char *message = "Hello from my ESP32 (via twilio)";

Twilio *twilio;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to WiFi network ;");
  Serial.print(ssid);
  Serial.println("'...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(500);
  }
  Serial.println("Connected!");

  twilio = new Twilio(account_sid, auth_token);

  delay(1000);
  String response;
  bool success = twilio->send_message(to_number, from_number, message, response);
  if (success) {
    Serial.println("Sent message successfully!");
  } else {
    Serial.println(response);
  }
}

void loop() {
  
}
