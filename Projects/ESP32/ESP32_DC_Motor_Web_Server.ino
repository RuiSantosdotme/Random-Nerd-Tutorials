/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-web-server-dc-motor-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Motor A pins
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//HTML and CSS to build the web page
const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
    <head>
      <title>ESP IOT DASHBOARD</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="stylesheet" type="text/css" href="style.css">
      <link rel="icon" type="image/png" href="favicon.png">
      <script src="https://kit.fontawesome.com/0294e3a09e.js" crossorigin="anonymous"></script>
      <style>
      html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
      }
      h1 {
          font-size: 1.8rem;
          color: white;
      }

      .topnav {
          overflow: hidden;
          background-color: #0A1128;
      }
      body {
          margin: 0;
      }
      .content {
          padding: 50px;
      }
      .card-grid {
          max-width: 800px;
          margin: 0 auto;
          display: grid;
          grid-gap: 2rem;
          grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      }
      .card {
          background-color: white;
          box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
      }
      .card-title {
          font-size: 1.2rem;
          font-weight: bold;
          color: #034078
      }
      .state {
          font-size: 1.2rem;
          color:#1282A2;
      }
      button {
          border: none;
          color: #FEFCFB;
          padding: 15px 32px;
          text-align: center;
          font-size: 16px;
          width: 150px;
          border-radius: 4px;
          transition-duration: 0.4s;
      }
      .button-on {
          background-color:#034078;
      }
      .button-on:hover {
          background-color: #1282A2;
      }
      .button-off {
          background-color:#858585;
      }
      .button-off:hover {
          background-color: #252524;
      }
      .button-stop {
          background-color:#5e0f0f;
          width: 100%;
      }
      .button-stop:hover {
          background-color: #9b332c;
      }
      </style>
    </head>
    <body>
      <div class="topnav">
        <h1>CONTROL DC MOTOR</h1>
      </div>
      <div class="content">
        <div class="card-grid">
          <div class="card">
            <p class="card-title"><i class="fa-solid fa-gear"></i> DC Motor A</p>
            <p>
              <a href="forward"><button class="button-on"><i class="fa-solid fa-arrow-up"></i> FORWARD</button></a>
              <a href="backward"><button class="button-off"><i class="fa-solid fa-arrow-down"></i> BACKWARD</button></a>
            </p>
            <p>
              <a href="stop"><button class="button-stop"><i class="fa-solid fa-stop"></i> STOP</button></a>
            </p>
          </div>
        </div>
      </div>
    </body>
</html>
)rawliteral";

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void moveForward(){
  Serial.println("Moving Forward");
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
}

void moveBackward(){
  Serial.println("Moving Backward");
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
}

void stopMotor(){
  Serial.print("Motor Stopped");
  digitalWrite(enable1Pin, LOW);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
}


void setup() {
  Serial.begin(115200);

  // Set motor pins as outputs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  initWiFi();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request) {
    moveForward();
    request->send(200, "text/html", index_html);
  }); 

  server.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request) {
    moveBackward();
    request->send(200, "text/html", index_html);
  });

  // Route to set GPIO state to LOW (inverted logic for ESP8266)
  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    stopMotor();
    request->send(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
  
}
