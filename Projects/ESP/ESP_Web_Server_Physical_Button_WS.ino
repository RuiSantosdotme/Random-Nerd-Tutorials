#include "Arduino.h"
/*********
 Rui Santos
 Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-web-server-physical-button/

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 *********/

// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char *ssid = "REPLACE_WITH_YOUR_SSID";
const char *password = "REPLACE_WITH_YOUR_PASSWORD";

const char *PARAM_INPUT_1 = "state";

const int output = 27;
const int buttonPin = 4;

// Variables will change:
int ledState = LOW;          // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM
		= R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <h4>Output - GPIO 2 - State <span id="outputState"></span></h4>
  <label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="output"><span
      class="slider"></span></label><script>
var connection = new WebSocket("ws://" + window.location.host + "/ws");

// When the connection is open, send some data to the server
connection.onopen = function () {
  console.log('WebSocket opened');
};

// Log errors
connection.onerror = function (error) {
  console.log('WebSocket Error ' + error);
};

// Log errors
connection.onclose = function (e) {
  console.log('WebSocket Closed ', e);
};

// Log messages from the server
connection.onmessage = function (e) {
  console.log("ws_output_state", e.data);
  var inputChecked;
  var outputStateM;
  if( e.data == 1){ 
    inputChecked = true;
    outputStateM = "On";
  }
  else { 
    inputChecked = false;
    outputStateM = "Off";
  }
  document.getElementById("output").checked = inputChecked;
  document.getElementById("outputState").innerHTML = outputStateM;
};

function toggleCheckbox(element) {
  if(element.checked){ connection.send('1'); }
  else { connection.send('0'); }
}
</script>
</body>
</html>
)rawliteral";


void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
		AwsEventType type, void *arg, uint8_t *data, size_t len) {
	if (type == WS_EVT_CONNECT) {
		//client connected
		Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
		client->printf("%d", !digitalRead(output));
		client->ping();
	} else if (type == WS_EVT_DISCONNECT) {
		//client disconnected
		Serial.printf("ws[%s][%u] disconnect\n", server->url(),
				client->id());
	} else if (type == WS_EVT_ERROR) {
		//error was received from the other end
		Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(),
				*((uint16_t*) arg), (char*) data);
	} else if (type == WS_EVT_PONG) {
		//pong message was received (in response to a ping request maybe)
		Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len,
				(len) ? (char*) data : "");
	} else if (type == WS_EVT_DATA) {
		//data packet
		AwsFrameInfo *info = (AwsFrameInfo*) arg;
		if (info->final && info->index == 0 && info->len == len) {
			//the whole message is in a single frame and we got all of it's data
			Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(),
					client->id(), (info->opcode == WS_TEXT) ? "text" : "binary",
					info->len);
			if (info->opcode == WS_TEXT) {
				data[len] = 0;
				int command = atoi((char*)data);
				Serial.printf("%s\n", (char*) data);
				ledState = !command;
				digitalWrite(output, ledState);
				server->printfAll("%d", !digitalRead(output));
			}
		}
	}
}

void setup() {
	// Serial port for debugging purposes
	Serial.begin(115200);

	pinMode(output, OUTPUT);
	digitalWrite(output, LOW);
	pinMode(buttonPin, INPUT);

	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}

	// Print ESP Local IP Address
	Serial.println(WiFi.localIP());

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/html", index_html);
	});

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	// Start server
	server.begin();
}

void loop() {
	static unsigned long lastWsCleanupTime = millis();
	static const unsigned long WS_CLEANUP_INTERVAL_MS = 5000; // ms between WebSocket cleanup
	// read the state of the switch into a local variable:
	int reading = digitalRead(buttonPin);

	// check to see if you just pressed the button
	// (i.e. the input went from LOW to HIGH), and you've waited long enough
	// since the last press to ignore any noise:

	// If the switch changed, due to noise or pressing:
	if (reading != lastButtonState) {
		// reset the debouncing timer
		lastDebounceTime = millis();
	}

	if ((millis() - lastDebounceTime) > debounceDelay) {
		// whatever the reading is at, it's been there for longer than the debounce
		// delay, so take it as the actual current state:

		// if the button state has changed:
		if (reading != buttonState) {
			buttonState = reading;

			// only toggle the LED if the new button state is HIGH
			if (buttonState == HIGH) {
				ledState = !ledState;
				digitalWrite(output, ledState);
				ws.printfAll("%d", !digitalRead(output));
			}
		}
	}

	// set the LED:
	digitalWrite(output, ledState);

	if ((millis() - lastWsCleanupTime) > WS_CLEANUP_INTERVAL_MS) {
		  ws.cleanupClients();
		  lastWsCleanupTime = millis();
	}

	// save the reading. Next time through the loop, it'll be the lastButtonState:
	lastButtonState = reading;
}
