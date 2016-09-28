/*
 * Random Nerd Tutorials - Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */
 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Wire.begin();
  dht.begin(); // initialize dht
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// initialize with the I2C addr 0x3C (for the 128x32)(initializing the display)
  Serial.begin(9600);
}
void displayTempHumid(){
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    display.clearDisplay(); // clearing the display
    display.setTextColor(WHITE); //setting the color
    display.setTextSize(1); //set the font size
    display.setCursor(5,0); //set the cursor coordinates
    display.print("Failed to read from DHT sensor!");
    return;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Humidity: "); 
  display.print(h);
  display.print(" %\t");
  display.setCursor(0,10);
  display.print("Temperature: "); 
  display.print(t);
  display.print(" C"); 
  display.setCursor(0,20);
  display.print("Temperature: "); 
  display.print(f);
  display.print(" F"); 
}
void loop()
{
  displayTempHumid();
  display.display();
}
