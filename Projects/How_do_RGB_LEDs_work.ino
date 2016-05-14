/*
 
 All the resources for this project:
 http://randomnerdtutorials.com/
 
*/

int redPin = 3;     // Red RGB pin -> D3
int greenPin = 5;   // Green RGB pin -> D5
int bluePin = 6;    // Blue RGB pin -> D6

int potRed = A0;     // Potentiometer controls Red pin -> A0
int potGreen = A1;   // Potentiometer controls Green pin -> A1
int potBlue = A2;    // Potentiometer controls Blue pin -> A2

void setup() {
  pinMode(redPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
  pinMode(greenPin, OUTPUT);
  
  pinMode(potRed, INPUT); 
  pinMode(potGreen, INPUT); 
  pinMode(potBlue, INPUT); 
}

void loop() {
  // Reads the current position of the potentiometer and converts 
  // to a value between 0 and 255 to control the according RGB pin with PWM
  // RGB LED COMMON ANODE
  analogWrite(redPin,(255./1023.)*analogRead(potRed));
  analogWrite(greenPin,(255./1023.)*analogRead(potGreen));
  analogWrite(bluePin,(255./1023.)*analogRead(potBlue));

  // Uncomment for RGB LED COMMON CATHODE
  /*
  analogWrite(redPin,255-(255./1023.)*analogRead(potRed));
  analogWrite(greenPin,255-(255./1023.)*analogRead(potGreen));
  analogWrite(bluePin,255-(255./1023.)*analogRead(potBlue));
  */
  
  delay(10);
}
