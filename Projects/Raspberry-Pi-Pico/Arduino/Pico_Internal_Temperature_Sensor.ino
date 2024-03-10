/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-internal-temperature-arduino/
*********/

float tempC;
float tempF;

void setup() {
  Serial.begin(115200);
}

void loop() {
  tempC = analogReadTemp(); // Get internal temperature
  tempF = tempC * 9.0 / 5.0 + 32.0; // Fahrenheit conversion
  Serial.print("Temperature Celsius (ºC): ");
  Serial.println(tempC);
  Serial.print("Temperature Fahrenheit (ºF): ");
  Serial.println(tempF);
  delay(1000);
}
