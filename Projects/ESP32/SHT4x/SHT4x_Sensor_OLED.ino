/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-sht41-temperature-humidity-arduino/
  Based on the example provided by the Adafruit_SHT4x Library
*/
#include <Adafruit_SH110X.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_SHT4x.h"
#include <Fonts/FreeSans9pt7b.h>

#define SCREEN_WIDTH 128    // OLED display width (pixels)
#define SCREEN_HEIGHT 64    // OLED display height (pixels)

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1   // Reset pin (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Declaration for the SHT4X connected to the default I2C pins
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

void setup() {
  Serial.begin(115200);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  // Clear the display
  display.clearDisplay();
  display.display();
  
  // Initialize SHT4x sensor
  Serial.println("Initializing SHT4x");
  if (! sht4.begin(&Wire)) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);
  
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(SSD1306_WHITE);  
}

void loop() {
  sensors_event_t humidity, temp;
  
  uint32_t timestamp = millis();
  // Get the latest temperature and humidity readings
  sht4.getEvent(&humidity, &temp);
  // Calculate the time it takes to get a new reading
  timestamp = millis() - timestamp;

  Serial.print("Temperature: "); 
  Serial.print(temp.temperature); 
  Serial.println(" degrees C");

  Serial.print("Humidity: "); 
  Serial.print(humidity.relative_humidity); 
  Serial.println("% rH");

  // Convert temperature to Fahrenheit
  float tempF = 1.8 * temp.temperature + 32;
  Serial.print("Temperature: ");
  Serial.print(tempF);
  Serial.println(" degrees F");

  Serial.print("Read duration (ms): ");
  Serial.println(timestamp);

  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("SHT41 Sensor");
  display.setCursor(0, 40);

  display.print("Temp: "); 
  display.print(temp.temperature,1); 
  display.println(" C");

  // Display temperature in Fahrenheit
  //display.print("Temp: "); 
  //display.print(tempF,1);
  //display.println(" F");

  display.print("Hum: "); 
  display.print(humidity.relative_humidity,1); 
  display.println(" %");
  display.display();
  yield();
  delay(100);
}
