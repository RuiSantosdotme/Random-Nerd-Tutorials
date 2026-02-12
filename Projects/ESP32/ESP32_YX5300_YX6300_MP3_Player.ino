/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-yx5300-yx6300-mp3-player-arduino/
*********/
#include <YX5300_ESP32.h>

// Connect to YX5300 Serial pins (RX and TX)
#define RX 16
#define TX 17

// Global variables for the pushbuttons
const uint8_t pinBtnPlay = 5;
const uint8_t pinBtnPrev = 18;
const uint8_t pinBtnNext = 19;

// Bool variable to track if the audio is playing or paused
volatile bool isPlaying = true;

// Variables for debouncing the pushbuttons
const unsigned long DEBOUNCE_DELAY = 500;  // in milliseconds
volatile unsigned long lastPressTime = 0;

// Analog input pin the potentiometer is attached to
const uint8_t potVolume = 4; 

// Auxiliary volume variables
#define MIN_VOLUME 0
#define MAX_VOLUME 30
int current_volume = 0;

// mp3 object
YX5300_ESP32 mp3; 

// Play/Pause Button Interrupt Service Routine (ISR)
void ARDUINO_ISR_ATTR buttonPlay() {
  unsigned long now = millis();
  if (now - lastPressTime > DEBOUNCE_DELAY) {    
    if(isPlaying) {
      mp3.pause();
      isPlaying = false;
    }
    else {
      mp3.resume();
      isPlaying = true;
    }
  }
  lastPressTime = now;
}

// Previous Track Button Interrupt Service Routine (ISR)
void ARDUINO_ISR_ATTR buttonPrev() {
  unsigned long now = millis();
  if (now - lastPressTime > DEBOUNCE_DELAY) {    
    mp3.prev();
    isPlaying = true;
  }
  lastPressTime = now;
}

// Next Track Button Interrupt Service Routine (ISR)
void ARDUINO_ISR_ATTR buttonNext() {
  unsigned long now = millis();
  if (now - lastPressTime > DEBOUNCE_DELAY) {    
    mp3.next();
    isPlaying = true;
  }
  lastPressTime = now;
}

void setup() {
  // Init pushbuttons with internal pullup resistor
  pinMode(pinBtnPlay, INPUT_PULLUP);
  pinMode(pinBtnPrev, INPUT_PULLUP);
  pinMode(pinBtnNext, INPUT_PULLUP);

  // Set interrupts ISR functions
  attachInterrupt(pinBtnPlay, buttonPlay, HIGH);
  attachInterrupt(pinBtnPrev, buttonPrev, HIGH);
  attachInterrupt(pinBtnNext, buttonNext, HIGH);

  // Initialize connection with the YX5300/YX6300 module
  mp3 = YX5300_ESP32(Serial2, RX, TX);
  
  // Shows the hex commands being sent to the mp3 device (includes helpful errors)
  Serial.begin(115200);
  mp3.enableDebugging();
  
  // Plays the first track stored on the SD Card (Track Formats .mp3/.wav | Frequencies 8-48 kHz)
  // SD Card folder and file structure, example folder name 01 and file name 001xxx.mp3:
  // 01/001xxx.mp3   01/002xxx.mp3   01/003xxx.mp3   02/004xxx.mp3
  mp3.playTrack(1);
}

void loop() {  
  // Read the potentiometer's value
  int raw_value = analogRead(potVolume);
  
  // Map the value to the min/max of the mp3 device's volume values 
  int new_volume = map(raw_value, 0, 4095, MIN_VOLUME, MAX_VOLUME);
  
  // Change the volume if the pot position has changed
  if(new_volume != current_volume) {
    // Update the mp3 device's volume
    mp3.setVolume(new_volume);
    // Update the current volume with the new volume
    current_volume = new_volume;
  }
}
