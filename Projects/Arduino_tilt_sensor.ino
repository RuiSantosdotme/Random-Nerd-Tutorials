/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */
 
 
int ledPin = 12;         
int sensorPin = 4;
int sensorValue;
int lastTiltState = HIGH;   // the previous reading from the tilt sensor
    
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
 
void setup(){
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}
 
void loop(){
  sensorValue = digitalRead(sensorPin);
  // If the switch changed, due to noise or pressing:
  if (sensorValue == lastTiltState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    lastTiltState = sensorValue;
  }
  digitalWrite(ledPin, lastTiltState);

  Serial.println(sensorValue);
  delay(500);
}
