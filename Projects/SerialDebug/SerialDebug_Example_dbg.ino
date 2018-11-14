// Example from ladyada.net/learn/arduino/lesson4.html

int a = 5;
int b = 10;
int c = 20;

// SerialDebug Library

// Disable all debug ? Good to release builds (production)
// as nothing of SerialDebug is compiled, zero overhead :-)
// For it just uncomment the DEBUG_DISABLED
//#define DEBUG_DISABLED true

// Disable SerialDebug debugger ? No more commands and features as functions and globals
// Uncomment this to disable it 
//#define DEBUG_DISABLE_DEBUGGER true

// Define the initial debug level here (uncomment to do it)
//#define DEBUG_INITIAL_LEVEL DEBUG_LEVEL_VERBOSE

// Disable auto function name (good if your debug yet contains it)
//#define DEBUG_AUTO_FUNC_DISABLED true

// Include SerialDebug
#include "SerialDebug.h" // Download SerialDebug library: https://github.com/JoaoLopesF/SerialDebug

void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);           // set up Serial library at 9600 bps

  printlnA("Here is some math: ");

  printA("a = ");
  printlnA(a);
  printA("b = ");
  printlnA(b);
  printA("c = ");
  printlnA(c);

  printA("a + b = ");       // add
  printlnA(a + b);

  printA("a * c = ");       // multiply
  printlnA(a * c);
  
  printA("c / b = ");       // divide
  printlnA(c / b);
  
  printA("b - c = ");       // subtract
  printlnA(b - c);

#ifndef DEBUG_DISABLE_DEBUGGER

  // Add Functions and global variables to SerialDebug

  // Add functions that can called from SerialDebug

  //debugAddFunctionVoid(F("function"), &function); // Example for function without args
  //debugAddFunctionStr(F("function"), &function); // Example for function with one String arg
  //debugAddFunctionInt(F("function"), &function); // Example for function with one int arg

  // Add global variables that can showed/changed from SerialDebug
  // Note: Only global, if pass local for SerialDebug, can be dangerous

  debugAddGlobalInt(F("a"), &a);
  debugAddGlobalInt(F("b"), &b);
  debugAddGlobalInt(F("c"), &c);

#endif // DEBUG_DISABLE_DEBUGGER

}

void loop()                     // we need this to be here even though its empty
{

  // SerialDebug handle
  // Notes: if in inactive mode (until receive anything from serial),
  // it show only messages of always or errors level type
  // And the overhead during inactive mode is very low
  // Only if not DEBUG_DISABLED

  debugHandle();

}
