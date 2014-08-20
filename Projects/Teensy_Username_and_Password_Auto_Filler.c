/*
 Created by Rui Santos
 
 All the resources for this project:
 http://randomnerdtutorials.com/
 
 Based on some Arduino code examples 
 */
 
//inlude the bounce library 
#include <Bounce.h>

// Creating Bounce objects for each button makes detecting changes very easy.
Bounce button3 = Bounce(3, 10);  
Bounce button4 = Bounce(4, 10);  
Bounce button5 = Bounce(5, 10);  

void setup() { 
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
} 

void loop() {
  // updates all the buttons
  // don't use long delays in the loop
  button3.update();
  button4.update();
  button5.update();
  
  // when you press the buttons
  if (button3.fallingEdge()) {
    Keyboard.print("usermame_1");
    Keyboard.set_key1(KEY_TAB);
    Keyboard.send_now();
    Keyboard.print("password_1"); 
    Keyboard.set_key1(KEY_ENTER);
    Keyboard.send_now();
  }
  if (button4.fallingEdge()) {
    Keyboard.print("usermame_2");
    Keyboard.set_key1(KEY_TAB);
    Keyboard.send_now();
    Keyboard.print("password_2"); 
    Keyboard.set_key1(KEY_ENTER);
    Keyboard.send_now();
  }
  if (button5.fallingEdge()) {
    Keyboard.print("usermame_3");
    Keyboard.set_key1(KEY_TAB);
    Keyboard.send_now();
    Keyboard.print("password_3"); 
    Keyboard.set_key1(KEY_ENTER);
    Keyboard.send_now();
  }
  
  // release all the keys
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.send_now();
}

