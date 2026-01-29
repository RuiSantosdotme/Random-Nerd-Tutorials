/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-tm1637-4-digit-7-segment-display-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <WiFi.h>
#include "time.h"
#include <TM1637Display.h>

// Replace with your network credentials
const char* ssid     = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// TM1637 pins
#define CLK 19
#define DIO 18

TM1637Display display(CLK, DIO);

// Timezone (Lisbon / Portugal)
// Change if needed - See list of timezones strings: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char* TZ_INFO = "WET0WEST,M3.5.0/1,M10.5.0";

// Init Wifi
void initWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
}

// Initialize Time with the specified timezone
void initTime() {
  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", TZ_INFO, 1);
  tzset();
}

// display the time on the screen
void displayTime() {

  // blinking colon
  static bool colon = false;
  colon = !colon;

  // get the current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  int hours   = timeinfo.tm_hour;   // get hours (0–23)
  int minutes = timeinfo.tm_min;    // get minutes (0–59)

  // move hours to the left two digits
  int hourPart = hours * 100;

  // combine hours and minutes into HHMM
  int value = hourPart + minutes;

  // control the colon (for blinking colon)
  uint8_t colonMask;
  if (colon == true) {
    colonMask = 0b01000000;   // turn colon ON
  } else {
    colonMask = 0;            // turn colon OFF
  }

  display.showNumberDecEx(
    value,
    colonMask,   // blink colon
    true         // leading zeros
  );
}

void setup() {
  Serial.begin(115200);

  display.setBrightness(7);
  display.clear();

  initWiFi();
  initTime();
}

void loop() {
  displayTime();
  delay(1000);
}
