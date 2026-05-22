/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at: https://RandomNerdTutorials.com/esp32-send-email-smtp-server-arduino-ide/  
  Based on the example provided by the ReadyMail library: https://github.com/mobizt/ReadyMail/
*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define ENABLE_SMTP
#define ENABLE_DEBUG
#include <ReadyMail.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Sender SMTP settings (GMAIL)
// Change if using a different provider
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

// Sender email, app password, and name
#define AUTHOR_EMAIL "SENDER_EMAIL@XXX.com"
#define AUTHOR_APP_PASS "EMAIL_APP_PASSWORD"
#define AUTHOR_NAME "ESP32"

//Recipient's email
#define RECIPIENT_EMAIL "RECIPIENTE_EMAIL@XXXX.com"
#define RECIPIENT_NAME "RECIPIENT_NAME"

WiFiClientSecure ssl_client;
SMTPClient smtp(ssl_client);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  ssl_client.setInsecure();

  auto statusCallback = [](SMTPStatus status) {
    Serial.println(status.text);
  };

  smtp.connect(SMTP_HOST, SMTP_PORT, statusCallback);

  if (smtp.isConnected()) {
    smtp.authenticate(AUTHOR_EMAIL, AUTHOR_APP_PASS, readymail_auth_password);

    SMTPMessage msg;

    msg.headers.add(rfc822_from, String(AUTHOR_NAME) + " <" + AUTHOR_EMAIL + ">");
    msg.headers.add(rfc822_to, String(RECIPIENT_NAME) + " <" + RECIPIENT_EMAIL + ">");
    msg.headers.add(rfc822_subject, "Hello from the ESP32");
    //msg.text.body("This is a plain text message.");
    msg.html.body("<html><body><h1>Hello!</h1></body></html>");
     
    // Set NTP config time
    /* For times east of the Prime Meridian use 0-12
    For times west of the Prime Meridian add 12 to the offset.
    Ex. American/Denver GMT would be -6. 6 + 12 = 18 */
    const int gmtOffset_sec = 0; //offset time in seconds
    const int daylightOffset_sec = 0; //daylight saving time offset in seconds

    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
    // Set timestamp for the email
    while (time(nullptr) < 100000) delay(100);
    msg.timestamp = time(nullptr);

    smtp.send(msg);
  }
}

void loop() {
  
}
