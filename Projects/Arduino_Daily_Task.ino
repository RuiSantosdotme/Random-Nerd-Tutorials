/*
 *
 * Complete project details at http://randomnerdtutorials.com    
 * Based on TimeAlarmExample from TimeAlarms library created by Michael Margolis
 *
 */

#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

const int led = 7; 

void setup() {
  // prepare pin as output
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  Serial.begin(9600);
  // wait for Arduino Serial Monitor
  while (!Serial) ; 
  
  // get and set the time from the RTC
  setSyncProvider(RTC.get);   
  if (timeStatus() != timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");     
  
  // to test your project, you can set the time manually 
  //setTime(8,29,0,1,1,11); // set time to Saturday 8:29:00am Jan 1 2011

  // create the alarms, to trigger functions at specific times
  Alarm.alarmRepeat(9,0,0,MorningAlarm);  // 9:00am every day
  Alarm.alarmRepeat(7,0,0,EveningAlarm);  // 7:00pm every day
}

void loop() {
  digitalClockDisplay();
  // wait one second between each clock display in serial monitor
  Alarm.delay(1000); 
}

// functions to be called when an alarm triggers
void MorningAlarm() {
  // write here the task to perform every morning
  Serial.println("Tturn light off");
  digitalWrite(led, LOW);
}
void EveningAlarm() {
  // write here the task to perform every evening
  Serial.println("Turn light on");
  digitalWrite(led, HIGH);
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}
void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
