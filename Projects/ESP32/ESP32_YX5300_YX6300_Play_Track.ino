/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-yx5300-yx6300-mp3-player-arduino/
  Example based on the library example by bluejunimo https://github.com/bluejunimo/YX5300_ESP32/
*********/
#include <YX5300_ESP32.h>

// Connect to YX5300 Serial pins (RX and TX)
#define RX 16
#define TX 17

// mp3 object
YX5300_ESP32 mp3;

void setup() {
  // Initialize connection with the YX5300/YX6300 module
  mp3 = YX5300_ESP32(Serial2, RX, TX);
  
  // Shows the hex commands being sent to the mp3 device (includes helpful errors)
  Serial.begin(115200);
  mp3.enableDebugging();

  // Plays the first track stored on the microSD Card (Track Formats .mp3/.wav | Frequencies 8-48 kHz)
  // MicroSD Card folder and file structure, example folder name 01 and file name 001xxx.mp3:
  // 01/001xxx.mp3   01/002xxx.mp3   01/003xxx.mp3   02/004xxx.mp3
  mp3.playTrack(1);
}

void loop() {
  // put your main code here, to run repeatedly:
}
