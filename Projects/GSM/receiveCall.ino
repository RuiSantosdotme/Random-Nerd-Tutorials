/*********
  Complete project details at http://randomnerdtutorials.com  
*********/

#include <SoftwareSerial.h>

// Configure software serial port
SoftwareSerial SIM900(7, 8); 
char incoming_char=0;

void setup() {
  // Arduino communicates with SIM900 GSM shield at a baud rate of 19200
  // Make sure that corresponds to the baud rate of your module
  SIM900.begin(19200); // for GSM shield
  // For serial monitor
  Serial.begin(19200); 
  // Give time to log on to network.
  delay(20000); 
  
  SIM900.print("AT+CLIP=1\r"); // turn on caller ID notification
  delay(100);
}

void loop() {
  // Display any text that the GSM shield sends out on the serial monitor
  if(SIM900.available() >0) {
    // Get the character from the cellular serial por
    // With an incomming call, a "RING" message is sent out
    incoming_char=SIM900.read();
    //  Check if the shield is sending a "RING" message
    if (incoming_char=='R') {
      delay(10);
      Serial.print(incoming_char);
      incoming_char=SIM900.read();
      if (incoming_char =='I') {
        delay(10);
        Serial.print(incoming_char);
        incoming_char=SIM900.read();
        if (incoming_char=='N') {
          delay(10);
          Serial.print(incoming_char);
          incoming_char=SIM900.read();
          if (incoming_char=='G') {
            delay(10);
            Serial.print(incoming_char);
            // If the message received from the shield is RING
            // Send ATA commands to answer the phone
            SIM900.print("ATA\r");
          }
        }
      }
    }
  }
}
