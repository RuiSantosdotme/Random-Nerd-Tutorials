/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Define power latch pin for ESP32 (GPIO 5) / ESP8266 (GPIO 5) / Arduino (Digital 5)
const int powerLatch = 5;

void setup() {
  // Define pin as an OUTPUT
  pinMode(powerLatch, OUTPUT); 
  
  // Keeps the circuit on
  digitalWrite(powerLatch, HIGH);
  // Waits for 10 seconds
  delay(10000);
  // Turns the power latch circuit off
  digitalWrite(powerLatch, LOW);
}

void loop() {
  
}
