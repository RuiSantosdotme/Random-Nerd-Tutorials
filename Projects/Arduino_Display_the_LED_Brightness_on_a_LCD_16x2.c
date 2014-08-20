/*
 Created by Rui Santos
 
 All the resources for this project:
 http://randomnerdtutorials.com/
 
 Based on some Arduino code examples 
*/

// include the library code
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int potPin = A0;       // Analog pin 0 for the LED brightness potentiometer
int ledPin = 6;        // LED Digital Pin with PWM
int potValue = 0;      // variable to store the value coming from the potentiometer
int brightness = 0;    // converts the potValue into a brightness 
int pBari = 0;         // progress bar
int i = 0;             // foor loop

//progress bar character for brightness
byte pBar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

void setup() {
  // setup our led as an OUTPUT
  pinMode(ledPin, OUTPUT); 
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD
  lcd.print(" LED Brightness");
  //Create the progress bar character
  lcd.createChar(0, pBar);
}

void loop() {
  // clears the LCD screen
  lcd.clear();
  // Print a message to the LCD
  lcd.print(" LED Brightness");
  //set the cursor to line number 2
  lcd.setCursor(0,1);
  // read the value from the potentiometer
  potValue = analogRead(potPin);        
  // turns the potValue into a brightness for the LED
  brightness=map(potValue, 0, 1024, 0, 255); 
  //lights up the LED according to the bightness
  analogWrite(ledPin, brightness); 
  // turns the brighness into a percentage for the bar 
  pBari=map(brightness, 0, 255, 0, 16);
  //prints the progress bar
  for (i=0; i<pBari; i++)
  {
    lcd.setCursor(i, 1);   
    lcd.write(byte(0));  
  }
  // delays 750 ms
  delay(750);        
}
