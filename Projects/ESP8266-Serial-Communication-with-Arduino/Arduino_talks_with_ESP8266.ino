/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */

// Send Data From Arduino to ESP8266 via Serial @ baud rate 9600
void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.print("HI!");
    delay(1000);
}
