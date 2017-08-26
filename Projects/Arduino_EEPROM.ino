/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */

#include <EEPROM.h>

const int buttonPin = 8;    // pushbutton pin
const int ledPin = 4;       // LED pin

int ledState;                // variable to hold the led state
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin


// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  // set input and output
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);

  // initialize serial monitor
  Serial.begin (9600);

  //check stored LED state on EEPROM using function defined at the end of the code
  checkLedState(); 
}

void loop() {
  // read the state of the switch into a local variable
  int reading = digitalRead(buttonPin);

  if(reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if(reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if(buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED state
  digitalWrite(ledPin, ledState);
  // save the current LED state in the EEPROM
  EEPROM.update(0, ledState);
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState
  lastButtonState = reading;
}

// Prints and upates the LED state
// when the Arduino board restarts or powers up
void checkLedState() {
   Serial.println("LED status after restart: ");
   ledState = EEPROM.read(0);
   if(ledState == 1) {
    Serial.println ("ON");
    digitalWrite(ledPin, HIGH);
   } 
   if(ledState == 0) {
    Serial.println ("OFF");
    digitalWrite(ledPin, LOW);
   }
}
