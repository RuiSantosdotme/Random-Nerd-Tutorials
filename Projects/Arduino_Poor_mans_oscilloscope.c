/* 
  Complete project details: http://randomnerdtutorials.com/arduino-poor-mans-oscilloscope/
*/

#define ANALOG_IN 0
 
void setup() {
  Serial.begin(9600); 
  //Serial.begin(115200); 
}
 
void loop() {
  int val = analogRead(ANALOG_IN);                                              
  Serial.write( 0xff );                                                         
  Serial.write( (val >> 8) & 0xff );                                            
  Serial.write( val & 0xff );
}
