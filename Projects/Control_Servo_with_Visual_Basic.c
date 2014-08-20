/*
 * Control a servo motor with Visual Basic 
 * Created by Rui Santos, http://randomnerdtutorials.com
*/
 
#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
 
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  Serial.begin(9600); //begins serial communication
} 
  
void loop() 
{ 
  int pos;
  if (Serial.available()){
    delay(100);
    while(Serial.available()>0){
      pos=Serial.read();     //reads the value sent from Visual Basic  
      if(pos=='0')
        myservo.write(90);   //rotates the servo 90 degrees (Left)
      else if(pos=='1')
        myservo.write(-90);  //rotates the servo 90 degrees (right)
      else if(pos=='2')
        myservo.write(180);  //rotates the servo 180 degrees (Left)
      else if(pos=='3')
        myservo.write(-180); //rotates the servo 180 degrees (right)     
    }
  } 
} 
