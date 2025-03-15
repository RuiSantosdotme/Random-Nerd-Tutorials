/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-wake-up-deep-sleep-external-alarms-ds3231/
*********/

#include <RTClib.h>
#include "driver/rtc_io.h"

// Define the DS3231 Interrupt pin (will wake-up the ESP32 - must be an RTC GPIO)
#define CLOCK_INTERRUPT_PIN              GPIO_NUM_4  // Only RTC IO are allowed

// LED for visual indication
const int ledPin = 2;

// Save how many times the ESP32 woke-up
RTC_DATA_ATTR int bootCount = 0;

// Instance for the RTC
RTC_DS3231 rtc;

// Set the alarm
DateTime alarm1Time = DateTime(2024, 12, 18, 12, 25, 0);

// Method to print the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void IRAM_ATTR onAlarm(){
  Serial.print("Alarm occurred!");
}

void setup() {
  Serial.begin(115200);
  pinMode (ledPin, OUTPUT);
  
  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Blink the LED when the ESP32 wakes-up
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);

  // Initialize the RTC
  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    Serial.flush();
    while (1) delay(10);
  }

  if(rtc.lostPower()) {
      // this will adjust to the date and time at compilation
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Uncomment if you need to define the time of the RTC
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // We don't need the 32K Pin, so disable it
  rtc.disable32K();

  // The alarm will trigger an interrupt
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);

  // Set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
  // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // Stop oscillating signals at SQW Pin otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  // Turn off alarm 2 (in case it isn't off already)
  // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
  rtc.disableAlarm(2);

  // Schedule an alarm
  if(!rtc.setAlarm1(alarm1Time, DS3231_A1_Minute)) {  // this mode triggers the alarm when the minutes match
      Serial.println("Error, alarm wasn't set!");
  }else {
      Serial.println("Alarm will happen at specified time");
  }
  
  // Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  // Configure external wake-up
  esp_sleep_enable_ext0_wakeup(CLOCK_INTERRUPT_PIN, 0);  //1 = High, 0 = Low
  // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
  // The RTC SQW pin is active low
  rtc_gpio_pulldown_dis(CLOCK_INTERRUPT_PIN);
  rtc_gpio_pullup_en(CLOCK_INTERRUPT_PIN);

  //Go to sleep now until an alarm fires
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
}

void loop() {
  // The code never reaches the loop, because the ESP32 goes to sleep at the end of setup
  Serial.print("This will never be printed!");
}
