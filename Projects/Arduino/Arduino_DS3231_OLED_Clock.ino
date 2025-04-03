/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/arduino-digital-clock-ds3231-oled/
*********/

#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SSD1306.h>

RTC_DS3231 rtc;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create OLED object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Days of the week array
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  Serial.begin(9600);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  // Clear the display
  display.clearDisplay();
  display.display();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Check if RTC lost power and set the time if necessary
  if (rtc.lostPower()) {
    Serial.println("RTC lost power! Setting the time...");
    // Set the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // Or, if you need to set the time for the first time, uncomment the following line:
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void loop() {
  // Get the current time from the RTC
  DateTime now = rtc.now();

  // Clear the OLED display buffer
  display.clearDisplay();

  // Display the date
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Date: ");
  display.print(now.year());
  display.print("-");
  if (now.month() < 10) display.print("0");
  display.print(now.month());
  display.print("-");
  if (now.day() < 10) display.print("0");
  display.print(now.day());

  // Display the time
  display.setCursor(0, 15);
  display.print("Time: ");
  if (now.hour() < 10) display.print("0");
  display.print(now.hour());
  display.print(":");
  if (now.minute() < 10) display.print("0");
  display.print(now.minute());
  display.print(":");
  if (now.second() < 10) display.print("0");
  display.print(now.second());

  // Display the day of the week
  display.setCursor(0, 30);
  display.print("Day: ");
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);

  // Display the temperature from DS3231
  display.setCursor(0, 45);
  display.print("Temp: ");
  display.print(rtc.getTemperature());
  display.cp437(true);
  display.write(167);
  display.print("C");

  // Display everything on the OLED
  display.display();

  delay(1000);
}
