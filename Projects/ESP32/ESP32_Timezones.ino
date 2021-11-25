// RTC demo for ESP32, that includes TZ and DST adjustments
// Get the POSIX style TZ format string from  https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
// Created by Hardy Maxa
// Complete project details at: https://RandomNerdTutorials.com/esp32-ntp-timezones-daylight-saving/

#include <WiFi.h>
#include "time.h"

const char * ssid="REPLACE_WITH_YOUR_SSID";
const char * wifipw="REPLACE_WITH_YOUR_PASSWORD";

void setTimezone(String timezone){
  Serial.printf("  Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone){
  struct tm timeinfo;

  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo)){
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time 1");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
}

void  startWifi(){
  WiFi.begin(ssid, wifipw);
  Serial.println("Connecting Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Wifi RSSI=");
  Serial.println(WiFi.RSSI());
}

void setTime(int yr, int month, int mday, int hr, int minute, int sec, int isDst){
  struct tm tm;

  tm.tm_year = yr - 1900;   // Set date
  tm.tm_mon = month-1;
  tm.tm_mday = mday;
  tm.tm_hour = hr;      // Set time
  tm.tm_min = minute;
  tm.tm_sec = sec;
  tm.tm_isdst = isDst;  // 1 or 0
  time_t t = mktime(&tm);
  Serial.printf("Setting time: %s", asctime(&tm));
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);
}

void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  startWifi();

  initTime("WET0WEST,M3.5.0/1,M10.5.0");   // Set for Melbourne/AU
  printLocalTime();
}

void loop() {
  int i;
  
  // put your main code here, to run repeatedly:
  Serial.println("Lets show the time for a bit.  Starting with TZ set for Melbourne/Australia");
  for(i=0; i<10; i++){
    delay(1000);
    printLocalTime();
  }
  Serial.println();
  Serial.println("Now - change timezones to Berlin");
  setTimezone("CET-1CEST,M3.5.0,M10.5.0/3");
  for(i=0; i<10; i++){
    delay(1000);
    printLocalTime();
  }

  Serial.println();
  Serial.println("Now - Lets change back to Lisbon and watch Daylight savings take effect");
  setTimezone("WET0WEST,M3.5.0/1,M10.5.0");
  printLocalTime();

  Serial.println();
  Serial.println("Now change the time.  1 min before DST takes effect. (1st Sunday of Oct)");
  Serial.println("AEST = Australian Eastern Standard Time. = UTC+10");
  Serial.println("AEDT = Australian Eastern Daylight Time. = UTC+11");
  setTime(2021,10,31,0,59,50,0);    // Set it to 1 minute before daylight savings comes in.
  
  for(i=0; i<20; i++){
    delay(1000);
    printLocalTime();
  }

  Serial.println("Now change the time.  1 min before DST should finish. (1st Sunday of April)");
  setTime(2021,3,28,1,59,50,1);    // Set it to 1 minute before daylight savings comes in.  Note. isDst=1 to indicate that the time we set is in DST.
  
  for(i=0; i<20; i++){
    delay(1000);
    printLocalTime();
  }

  // Now lets watch the time and see how long it takes for NTP to fix the clock
  Serial.println("Waiting for NTP update (expect in about 1 hour)");
  while(1) {
    delay(1000);
    printLocalTime();
  }
}
