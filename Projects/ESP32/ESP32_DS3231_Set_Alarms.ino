/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-ds3231-real-time-clock-arduino/
*********/

// Example based on the RTClib: implementation of an alarm using DS3231 https://github.com/adafruit/RTClib
#include <RTClib.h>
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// the pin that is connected to SQW
#define CLOCK_INTERRUPT_PIN 4

// LED for visual indication
const int ledPin = 2;

// set the alarms
// (year, month, day, hour, minutes, seconds)
DateTime alarm1Time = DateTime(2024, 12, 18, 12, 49, 0);
DateTime alarm2Time = DateTime(2024, 12, 18, 11, 10, 0);

void printCurrentTime(){
  // Get the current time from the RTC
  DateTime now = rtc.now();
  
  // Getting each time field in individual variables
  // And adding a leading zero when needed;
  String yearStr = String(now.year(), DEC);
  String monthStr = (now.month() < 10 ? "0" : "") + String(now.month(), DEC);
  String dayStr = (now.day() < 10 ? "0" : "") + String(now.day(), DEC);
  String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour(), DEC); 
  String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute(), DEC);
  String secondStr = (now.second() < 10 ? "0" : "") + String(now.second(), DEC);
  String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];

  // Complete time string
  String formattedTime = dayOfWeek + ", " + yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minuteStr + ":" + secondStr;

  // Print the complete formatted time
  Serial.println(formattedTime);
}

void onAlarm() {
  Serial.println("Alarm occurred!");
  // toggle the current LED state
  int state = digitalRead(ledPin);
  digitalWrite(ledPin, !state);
}

void setup() {
  Serial.begin(115200);
  pinMode (ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // initializing the rtc
  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    Serial.flush();
    while (1) delay(10);
  }

  if(rtc.lostPower()) {
    // this will adjust to the date and time at compilation
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // Uncomment if you need to adjust the time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //we don't need the 32K Pin, so disable it
  rtc.disable32K();

  // Trigger an interrupt when the alarm happens
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);

  // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
  // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // stop oscillating signals at SQW Pin, otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  // turn off alarm 2 (in case it isn't off already)
  // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
  rtc.disableAlarm(2);

  // Schedule Alarm1 to fire when the minutes match
  if(!rtc.setAlarm1(alarm1Time, DS3231_A1_Minute)) {  // this mode triggers the alarm when the minutes match
    Serial.println("Error, alarm wasn't set!");
  }else {
    Serial.println("Alarm 1 will happen at specified time");
  }
}

void loop() {
  // print current date and time
  printCurrentTime();

  // Get Details about the alarm1
  DateTime alarm1 = rtc.getAlarm1();
  Ds3231Alarm1Mode alarm1mode = rtc.getAlarm1Mode();
  char alarm1Date[12] = "DD hh:mm:ss";
  alarm1.toString(alarm1Date);
  Serial.print("[Alarm1: ");
  Serial.print(alarm1Date);
  Serial.print(", Mode: ");
  switch (alarm1mode) {
    case DS3231_A1_PerSecond: Serial.print("PerSecond"); break;
    case DS3231_A1_Second: Serial.print("Second"); break;
    case DS3231_A1_Minute: Serial.print("Minute"); break;
    case DS3231_A1_Hour: Serial.print("Hour"); break;
    case DS3231_A1_Date: Serial.print("Date"); break;
    case DS3231_A1_Day: Serial.print("Day"); break;
  }
  // the value at SQW-Pin (because of pullup 1 means no alarm)
  Serial.print("] SQW: ");
  Serial.print(digitalRead(CLOCK_INTERRUPT_PIN));
  // whether a alarm fired
  Serial.print(" Fired: ");
  Serial.print(rtc.alarmFired(1));

  // Only one alarm can be set at a time, reset alarm 1 and activate alarm 2
  // resetting SQW and alarm 1 flag
  // the next alarm could now be configurated
  if (rtc.alarmFired(1)) {
      rtc.clearAlarm(1);
      Serial.println(" - Alarm cleared");
      
      // Set Alarm 2
      if(!rtc.setAlarm2(alarm2Time, DS3231_A2_Minute)) {  // this mode triggers the alarm when the minutes match
        Serial.println("Error, alarm wasn't set!");
      }else {
        Serial.println("Alarm 2 will happen at specified time");
      }

    // Get Details about the alarm2
    DateTime alarm1 = rtc.getAlarm2();
    Ds3231Alarm2Mode alarm2mode = rtc.getAlarm2Mode();
    char alarm2Date[12] = "DD hh:mm:ss";
    alarm1.toString(alarm2Date);
    Serial.print("[Alarm2: ");
    Serial.print(alarm2Date);
    Serial.print(", Mode: ");
    switch (alarm2mode) {
      case DS3231_A2_PerMinute: Serial.print("Every Minute"); break;
      case DS3231_A2_Minute: Serial.print("Minute"); break;
      case DS3231_A2_Hour: Serial.print("Hour"); break;
      case DS3231_A2_Date: Serial.print("Date"); break;
      case DS3231_A2_Day: Serial.print("Day"); break;
    }
    // the value at SQW-Pin (because of pullup 1 means no alarm)
    Serial.print("] SQW: ");
    Serial.print(digitalRead(CLOCK_INTERRUPT_PIN));
    // whether a alarm fired
    Serial.print(" Fired: ");
    Serial.print(rtc.alarmFired(1)); 
  }

  Serial.println();
  delay(2000);
}
