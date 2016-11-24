/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

// Relay pin is controlled with D8. The active wire is connected to Normally Closed and common
int relay = 8;
volatile byte relayState = LOW;

// PIR Motion Sensor is connected to D2.
int PIRInterrupt = 2;

// Timer Variables
long lastDebounceTime = 0;  
long debounceDelay = 10000;

void setup() {
  // Pin for relay module set as output
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  // PIR motion sensor set as an input
  pinMode(PIRInterrupt, INPUT);
  // Triggers detectMotion function on rising mode to turn the relay on, if the condition is met
  attachInterrupt(digitalPinToInterrupt(PIRInterrupt), detectMotion, RISING);
  // Serial communication for debugging purposes
  Serial.begin(9600);
}

void loop() {
  // If 10 seconds have passed, the relay is turned off
  if((millis() - lastDebounceTime) > debounceDelay && relayState == HIGH){
    digitalWrite(relay, HIGH);
    relayState = LOW;
    Serial.println("OFF");
  }
  delay(50);
}

void detectMotion() {
  Serial.println("Motion");
  if(relayState == LOW){
    digitalWrite(relay, LOW);
  }
  relayState = HIGH;  
  Serial.println("ON");
  lastDebounceTime = millis();
}
