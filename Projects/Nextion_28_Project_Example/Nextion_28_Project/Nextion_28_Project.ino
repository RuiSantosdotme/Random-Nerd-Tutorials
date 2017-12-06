/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */

#include "Nextion.h"

#include "DHT.h"
#define DHTPIN 4     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// LED pins
const int led1 = 8;
const int led2 = 9;

// Declare your Nextion objects - Example (page id = 0, component id = 1, component name = "b0") 
NexText tState = NexText(0, 4, "tState"); 
NexButton bOn = NexButton(0, 2, "bOn");
NexButton bOff = NexButton(0, 3, "bOff");
NexSlider h0 = NexSlider(0, 5, "h0");
NexText tSlider = NexText(0, 6, "tSlider");
NexText tTempC = NexText(1, 5, "tTempC");
NexText tTempF = NexText(1, 4, "tTempF");
NexProgressBar jHumidity = NexProgressBar(1, 8, "jHumidity");
NexText tHumidity = NexText(1, 9, "tHumidity");
NexButton bUpdate = NexButton(1,10, "bUpdate");

// Register a button object to the touch event list.  
NexTouch *nex_listen_list[] = {
  &bOn,
  &bOff,
  &h0,
  &bUpdate,
  NULL
};
 
/*
 * Button bOn component pop callback function. 
 * When the ON button is released, the LED turns on and the state text changes. 
 */
void bOnPopCallback(void *ptr) {
  tState.setText("State: on");
  digitalWrite(led1, HIGH);
}

/*
 * Button bOff component pop callback function. 
 * When the OFF button is released, the LED turns off and the state text changes. 
 */
void bOffPopCallback(void *ptr) {
  tState.setText("State: off");
  digitalWrite(led1, LOW);
}

/*
 * Slider h0 component pop callback function. 
 * When the slider is released, the LED brightness changes and the slider text changes. 
 */
void h0PopCallback(void *ptr) {
  uint32_t number = 0;
  char temp[10] = {0};
  // change text with the current slider value
  h0.getValue(&number);
  utoa(number, temp, 10);
  tSlider.setText(temp);
  // change LED brightness
  analogWrite(led2, number); 
}

/*
 * Button bUpdate component pop callback function. 
 * When the UPDATE button is released, the temperature and humidity readings are updated. 
 */
void bUpdatePopCallback(void *ptr) {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    return;
  }
  // Update temperature in Celsius
  static char temperatureCTemp[6];
  dtostrf(t, 6, 2, temperatureCTemp);
  tTempC.setText(temperatureCTemp);

  // Update humidity percentage text and progress bar
  char hTemp[10] = {0}; 
  utoa(int(h), hTemp, 10);
  tHumidity.setText(hTemp);
  jHumidity.setValue(int(h));

  // Update temperature in Fahrenheit
  static char temperatureFTemp[6];
  dtostrf(f, 6, 2, temperatureFTemp);
  tTempF.setText(temperatureFTemp);
}

void setup(void) {    
  dht.begin();
  Serial.begin(9600);
    
  // You might need to change NexConfig.h file in your ITEADLIB_Arduino_Nextion folder
  // Set the baudrate which is for debug and communicate with Nextion screen
  nexInit();

  // Register the pop event callback function of the components
  bOn.attachPop(bOnPopCallback, &bOn);
  bOff.attachPop(bOffPopCallback, &bOff);
  h0.attachPop(h0PopCallback);
  bUpdate.attachPop(bUpdatePopCallback, &bUpdate);
    
  // Set LEDs as outputs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop(void) {   
  /*
   * When a pop or push event occured every time,
   * the corresponding component[right page id and component id] in touch event list will be asked.
   */
  nexLoop(nex_listen_list);
}
