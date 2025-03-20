/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-light-sleep-arduino/
*********/

#include <driver/uart.h>

int counter = 0;
const int ledPin = 2;         // GPIO pin for onboard LED
String receivedMessage = "";  // Variable to store the complete message

// Method to print the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    case ESP_SLEEP_WAKEUP_UART:     Serial.println("Wakeup caused by UART"); break;
    default:                        Serial.printf("Wakeup was not caused by light sleep: %d\n", wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  delay(1000);  //Take some time to open up the Serial Monitor

  // Enable UART wake-up from light sleep
  uart_set_wakeup_threshold(UART_NUM_0, 3);  // 3 edges on U0RXD to wakeup
  esp_sleep_enable_uart_wakeup(0);           // UART0 (default Serial (includes Serial Monitor))
}

void loop() {
  Serial.printf("Counter: %d\n", counter);
  counter++;

  digitalWrite(ledPin, HIGH); // LED on to indicate wake-up
  delay(5000);
  digitalWrite(ledPin, LOW); // LED on to indicate wake-up

  Serial.println("Going into light sleep...");
  delay(500);
  esp_light_sleep_start(); 
  
  Serial.println("----------------------");
  Serial.println("Returning from light sleep");  
  delay(2000);
  
  print_wakeup_reason();
   // Clear the internal wake-up indication by sending some extra data
  Serial.write(' ');   // Send a single space character

 while (Serial.available()) {
    char incomingChar = Serial.read();  // Read each character from the buffer
    
    if (incomingChar == '\n') {  // Check if the user pressed Enter (new line character)
      // Print the message
      Serial.print("You sent: ");
      Serial.println(receivedMessage);
      
      // Clear the message buffer for the next input
      receivedMessage = "";
    } else {
      // Append the character to the message string
      receivedMessage += incomingChar;
    }
  }  
}