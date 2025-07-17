/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/getting-started-esp32-c3-super-mini/
*********/

// ESP32 C3 Super Mini on-board LED (works with inverted logic)
const int ledPin = 8; 

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  Serial.begin(115200);
  digitalWrite(ledPin, HIGH); 
  Serial.println("LED OFF");
  delay(1000);                     
  digitalWrite(ledPin, LOW); 
  Serial.println("LED ON");  
  delay(5000);                     
}