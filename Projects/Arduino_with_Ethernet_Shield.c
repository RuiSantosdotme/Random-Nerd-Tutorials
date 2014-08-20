/*
 Created by Rui Santos
 Visit: http://randomnerdtutorials.com for more arduino projects

 Arduino with Ethernet Shield
 */

#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h> 
int led = 4;
Servo microservo; 
int pos = 0; 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 1, 178 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 1, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port     
String readString;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(led, OUTPUT);
  microservo.attach(7);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         //if HTTP request has ended
         if (c == '\n') {          
           Serial.println(readString); //print to serial monitor for debuging
     
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Random Nerd Tutorials Project</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Random Nerd Tutorials Project</H1>");
           client.println("<hr />");
           client.println("<br />");  
           client.println("<H2>Arduino with Ethernet Shield</H2>");
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\">Turn On LED</a>");
           client.println("<a href=\"/?button1off\"\">Turn Off LED</a><br />");   
           client.println("<br />");     
           client.println("<br />"); 
           client.println("<a href=\"/?button2on\"\">Rotate Left</a>");
           client.println("<a href=\"/?button2off\"\">Rotate Right</a><br />"); 
           client.println("<p>Created by Rui Santos. Visit http://randomnerdtutorials.com for more projects!</p>");  
           client.println("<br />"); 
           client.println("</BODY>");
           client.println("</HTML>");
     
           delay(1);
           //stopping client
           client.stop();
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?button1on") >0){
               digitalWrite(led, HIGH);
           }
           if (readString.indexOf("?button1off") >0){
               digitalWrite(led, LOW);
           }
           if (readString.indexOf("?button2on") >0){
                for(pos = 0; pos < 180; pos += 3)  // goes from 0 degrees to 180 degrees 
                {                                  // in steps of 1 degree 
                  microservo.write(pos);              // tell servo to go to position in variable 'pos' 
                  delay(15);                       // waits 15ms for the servo to reach the position 
                } 
           }
           if (readString.indexOf("?button2off") >0){
                for(pos = 180; pos>=1; pos-=3)     // goes from 180 degrees to 0 degrees 
                {                                
                  microservo.write(pos);              // tell servo to go to position in variable 'pos' 
                  delay(15);                       // waits 15ms for the servo to reach the position 
                } 
           }
            //clearing string for next read
            readString="";  
           
         }
       }
    }
}
}

