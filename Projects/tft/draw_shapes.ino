/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */
 
// include TFT and SPI libraries
#include <TFT.h>  
#include <SPI.h>

// pin definition for Arduino UNO
#define cs   10
#define dc   9
#define rst  8


// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

void setup() {

  //initialize the library
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
}

void loop() {

  //generate a random color
  int redRandom = random(0, 255);
  int greenRandom = random (0, 255);
  int blueRandom = random (0, 255);
  
  // set the color for the figures
  TFTscreen.stroke(redRandom, greenRandom, blueRandom);

  // light up a single point
  TFTscreen.point(80,64);
  // wait 200 miliseconds until change to next figure
  delay(500);

  // draw a line
  TFTscreen.line(0,64,160,64);
  delay(500);

  //draw a square
  TFTscreen.rect(50,34,60,60);
  delay(500);
    
  //draw a circle
  TFTscreen.circle(80,64,30);
  delay(500);

  //erase all figures
  TFTscreen.background(0,0,0);
}
