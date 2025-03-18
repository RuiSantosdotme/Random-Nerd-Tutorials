/*  
  Rui Santos & Sara Santos - Random Nerd Tutorials
  https://RandomNerdTutorials.com/esp32-esp8266-run-daily-task/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <time.h>

// Replace with your network credentials
const char* ssid     = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Timezone string for your region, example: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char* timezone = "WET0WEST,M3.5.0/1,M10.5.0"; // WET0WEST,M3.5.0/1,M10.5.0 = Europe/Lisbon

// Time that the daily task runs in 24 hour format
const int taskHour = 16;   // Hour example in 24 hour format: 16 = 4 PM
const int taskMinute = 5;  // 5 minutes

// Store the day when the task last ran to ensure it only runs once per day
int lastRunDay = -1;

unsigned long lastNTPUpdate = 0; // Timestamp for the last NTP sync
const unsigned long ntpSyncInterval = 30 * 60 * 1000; // Sync every 30 minutes (in ms)

void syncTime() {
  Serial.print("Synchronizing time with NTP server...");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // UTC offset set to 0
  time_t now = time(nullptr);
  while (now < 24 * 3600) { // Wait until time is valid
    delay(100);
    now = time(nullptr);
  }
  Serial.println(" Time synchronized!");
  
  // Set timezone
  setenv("TZ", timezone, 1);
  tzset();

  lastNTPUpdate = millis(); // Record the time of the last sync
}

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  syncTime();
}

void loop() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  // Current time and date
  Serial.printf("Current time: %02d:%02d:%02d, Date: %04d-%02d-%02d\n",
              timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
              timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

  // Check if it's time to run the daily task
  if (timeinfo.tm_hour == taskHour && timeinfo.tm_min == taskMinute && lastRunDay != timeinfo.tm_mday) {
    dailyTask();
    // Set the day to ensure it only runs once per day
    lastRunDay = timeinfo.tm_mday;
  }

  // Resynchronize with NTP every 30 minutes
  if (millis() - lastNTPUpdate > ntpSyncInterval) {
    syncTime();
  }

  delay(1000); // Run loop every second
}

void dailyTask() {
  Serial.println("#########\nDoing daily task...\n#########");
  // ENTER YOUR TASK HERE
}
