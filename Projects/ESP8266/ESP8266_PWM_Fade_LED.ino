/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

const int ledPin = 2; 

void setup() {
  
}

void loop() {
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle < 1023; dutyCycle++){   
    // changing the LED brightness with PWM
    analogWrite(ledPin, dutyCycle);
    delay(1);
  }

  // decrease the LED brightness
  for(int dutyCycle = 1023; dutyCycle > 0; dutyCycle--){
    // changing the LED brightness with PWM
    analogWrite(ledPin, dutyCycle);
    delay(1);
  }
}
