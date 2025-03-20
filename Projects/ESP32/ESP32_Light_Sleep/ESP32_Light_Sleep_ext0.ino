/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-light-sleep-arduino/
*********/

int counter = 0;
const int ledPin = 2;            // GPIO pin for onboard LED
#define buttonPin  GPIO_NUM_27   // Connect a pushbutton to GPIO 27

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLDOWN); // pull-down resistor

    // Enable wake-up by EXT0 using the button on GPIO 27
    esp_err_t result = esp_sleep_enable_ext0_wakeup(buttonPin, 1); // 1 = wake up on HIGH signal

    if (result == ESP_OK) {
        Serial.println("EXT0 Wake-Up set successfully as wake-up source.");
    } else {
        Serial.println("Failed to set EXT0 Wake-Up as wake-up source.");
    }
}

void loop() {
    Serial.printf("Counter: %d\n", counter);
    counter++;

    digitalWrite(ledPin, HIGH); // LED on to indicate wake-up
    delay(5000);
    digitalWrite(ledPin, LOW);  // Turn off LED before going to sleep

    Serial.println("Going into light sleep mode");
    delay(500);
    esp_light_sleep_start();    // Enter light sleep
    Serial.println("Returning from light sleep");
}