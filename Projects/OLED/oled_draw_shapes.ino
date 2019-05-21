/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(64, 32, WHITE);
  display.display();
  delay(3000);

  // Draw line
  display.clearDisplay();
  display.drawLine(0, 0, 127, 20, WHITE);
  display.display();
  delay(3000);
  
  // Draw rectangle
  display.clearDisplay();
  display.drawRect(30, 10, 50, 30, WHITE);
  display.display();
  delay(3000);
  // Fill rectangle
  display.fillRect(30, 10, 50, 30, WHITE);
  display.display();
  delay(3000);

  // Draw round rectangle
  display.clearDisplay();
  display.drawRoundRect(10, 10, 30, 50, 2, WHITE);
  display.display();
  delay(3000);
  // Fill round rectangle
  display.clearDisplay();
  display.fillRoundRect(10, 10, 30, 50, 2, WHITE);
  display.display();
  delay(3000);
  
  // Draw circle
  display.clearDisplay();
  display.drawCircle(64, 32, 10, WHITE);
  display.display();
  delay(3000);
  // Fill circle
  display.fillCircle(64, 32, 10, WHITE);
  display.display();
  delay(3000);
  
  // Draw triangle
  display.clearDisplay();
  display.drawTriangle(10, 10, 55, 20, 5, 40, WHITE);
  display.display();
  delay(3000);
  // Fill triangle
  display.fillTriangle(10, 10, 55, 20, 5, 40, WHITE);
  display.display();
  delay(3000);

  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(3000);
  display.invertDisplay(false);
  delay(3000);
}

void loop() {
  
}
