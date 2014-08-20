/*
 Modified by Rui Santos
 For more Arduino Projects: http://randomnerdtutorials.com 
 
 SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
    
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 based on Tom Igoe example.
 */
 
#include <SD.h>
float tempK;  //stores kelvin temperature
int sensor;   //stores sensor value everytime we use the analog read function
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4;
int analogPin=0;      //help us read the analogpin of our INPUTS
void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  //Defines Photoresistor as an INPUT on PIN Number 0
  pinMode(0, INPUT);
  //Defines Temperature sensor as an INPUT on PIN Number 1
  pinMode(1, INPUT);
}

void loop()
{
  // make a string for assembling the data to log:
  String dataString = "";
  // reads three sensors and append to the string:
  for (analogPin = 0; analogPin < 3; analogPin++){
    //reads the photoresistor on PIN0
    if( analogPin==0){
      sensor = analogRead(analogPin);
    }
    //reads the kelvin Tempeature on PIN1 
    //then converts our tempeature to Degrees
    else if(analogPin==1){  
      //reads temperature and converts to kelvin
      tempK = (((analogRead(analogPin)/ 1023.0) * 5.0) * 100.0);
      //Converts Kelvin to Celsius minus 1.5 degrees error
      sensor = tempK - 273.0; 
    }
    //reads the Kelvin temperature on PIN1
    //then converts our tempeature to Farenheit
    else{
      //reads temperature and converts to kelvin
      tempK = (((analogRead(analogPin-1)/ 1023.0) * 5.0) * 100.0);
      //Converts Kelvin to Farenheit
      sensor = ((tempK - 2.5) * 9 / 5) - 459.67;
    }
    //stores our values
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ","; 
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("data.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}
