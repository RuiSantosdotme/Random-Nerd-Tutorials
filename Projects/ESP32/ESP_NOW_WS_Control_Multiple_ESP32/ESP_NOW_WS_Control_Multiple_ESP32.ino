/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-esp-now-web-server-multiple-boards/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <esp_now.h>

// REPLACE WITH YOUR WI-FI CREDENTIALS
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// REPLACE WITH YOUR RECEIVER'S MAC ADDRESS
uint8_t board1Address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t board2Address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure to hold GPIO state
typedef struct struct_message {
    int gpio;
    bool state;
} struct_message;

struct_message board1Data;
struct_message board2Data;

// Confirmed delivered states
bool board1Btn1 = false;
bool board1Btn2 = false;
bool board2Btn1 = false;
bool board2Btn2 = false;

// Pending messages delivery tracking
struct PendingUpdate {
  bool active = false;
  int board;
  int gpio;
  bool newState;
  bool oldState;
};

PendingUpdate pendingUpdate;

// GPIO pin definitions
const int board1Pin1 = 2;
const int board1Pin2 = 4;
const int board2Pin1 = 20;
const int board2Pin2 = 21;

// Global variables
esp_now_peer_info_t peerInfo;
AsyncWebServer server(80);

// Function to get a pointer to the state variable based on board and GPIO
bool* 
(int board, int gpio) {
  if (board == 1 && gpio == board1Pin1) return &board1Btn1;
  if (board == 1 && gpio == board1Pin2) return &board1Btn2;
  if (board == 2 && gpio == board2Pin1) return &board2Btn1;
  if (board == 2 && gpio == board2Pin2) return &board2Btn2;
  return nullptr;
}

// ESP-NOW callback function when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Packet to: ");
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  if (!pendingUpdate.active) return;

  // Ensure the pending update corresponds to the correct board and GPIO
  bool* statePtr = getStatePtr(pendingUpdate.board, pendingUpdate.gpio);
  if (statePtr == nullptr) {
    pendingUpdate.active = false;
    return;
  }

  // Commit or revert the state based on delivery status
  if (status == ESP_NOW_SEND_SUCCESS) {
    *statePtr = pendingUpdate.newState;
    Serial.printf("State committed: board=%d gpio=%d state=%d\n",
                  pendingUpdate.board, pendingUpdate.gpio, pendingUpdate.newState);
  } else {
    *statePtr = pendingUpdate.oldState;
    Serial.printf("Delivery failed — state reverted: board=%d gpio=%d back to %d\n",
                  pendingUpdate.board, pendingUpdate.gpio, pendingUpdate.oldState);
  }
  pendingUpdate.active = false;
}

// Function to send ESP-NOW message
esp_err_t sendEspNowMessage(const uint8_t *boardMacAddress, const struct_message &message) {
  esp_err_t result = esp_now_send(boardMacAddress, (const uint8_t *) &message, sizeof(message));
  // Handle the result of the send operation
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    Serial.println("Error: ESP-NOW not initialized");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Error: Peer not found");
  } else {
    Serial.printf("Error sending the data: %d\n", result);
  }
  return result;
}

// HTML Web Page
String renderHTML() {
  String html = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP32 - ESP-NOW Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0; padding: 20px; background: #f4f4f4; }
    h2 { color: #333; }
    .card { background: white; padding: 20px; margin: 15px auto; max-width: 420px; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); }
    .switch-container { margin: 15px 0; display: flex; align-items: center; justify-content: space-between; padding: 10px 0; }
    .switch-label { font-size: 18px; font-weight: 500; }
    .switch { position: relative; display: inline-block; width: 60px; height: 34px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider {
      position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0;
      background-color: #ccc; transition: .4s; border-radius: 34px;
    }
    .slider:before {
      position: absolute; content: ""; height: 26px; width: 26px; left: 4px; bottom: 4px;
      background-color: white; transition: .4s; border-radius: 50%;
    }
    input:checked + .slider { background-color: #4CAF50; }
    input:checked + .slider:before { transform: translateX(26px); }
  </style>
</head>
<body>
  <h2>ESP32 - ESP-NOW Controller</h2>
)rawliteral";

  html += R"rawliteral(
  <div class="card">
    <h3>Board #1</h3>
    <div class="switch-container">
      <span class="switch-label">GPIO )rawliteral";
  html += String(board1Pin1);
  html += R"rawliteral(</span>
      <label class="switch">
        <input type="checkbox" id="b1Btn1" onchange="toggleSwitch(1,)rawliteral";
  html += String(board1Pin1);
  html += R"rawliteral(, this)">
        <span class="slider"></span>
      </label>
    </div>
    <div class="switch-container">
      <span class="switch-label">GPIO )rawliteral";
  html += String(board1Pin2);
  html += R"rawliteral(</span>
      <label class="switch">
        <input type="checkbox" id="b1Btn2" onchange="toggleSwitch(1,)rawliteral";
  html += String(board1Pin2);
  html += R"rawliteral(, this)">
        <span class="slider"></span>
      </label>
    </div>
  </div>
)rawliteral";

  html += R"rawliteral(
  <div class="card">
    <h3>Board #2</h3>
    <div class="switch-container">
      <span class="switch-label">GPIO )rawliteral";
  html += String(board2Pin1);
  html += R"rawliteral(</span>
      <label class="switch">
        <input type="checkbox" id="b2Btn1" onchange="toggleSwitch(2,)rawliteral";
  html += String(board2Pin1);
  html += R"rawliteral(, this)">
        <span class="slider"></span>
      </label>
    </div>
    <div class="switch-container">
      <span class="switch-label">GPIO )rawliteral";
  html += String(board2Pin2);
  html += R"rawliteral(</span>
      <label class="switch">
        <input type="checkbox" id="b2Btn2" onchange="toggleSwitch(2,)rawliteral";
  html += String(board2Pin2);
  html += R"rawliteral(, this)">
        <span class="slider"></span>
      </label>
    </div>
  </div>
)rawliteral";

  html += R"rawliteral(
  <script>
    function applyStates(states) {
      document.getElementById('b1Btn1').checked = states.b1Btn1 || false;
      document.getElementById('b1Btn2').checked = states.b1Btn2 || false;
      document.getElementById('b2Btn1').checked = states.b2Btn1 || false;
      document.getElementById('b2Btn2').checked = states.b2Btn2 || false;
    }

    async function loadStates() {
      try {
        const response = await fetch('/states');
        const states = await response.json();
        applyStates(states);
      } catch (err) {
        console.error("Failed to load states:", err);
      }
    }

    function toggleSwitch(board, gpio, element) {
      if (!element) {
        console.error("Element is undefined");
        return;
      }
      const state = element.checked;

      fetch(`/control?board=${board}&gpio=${gpio}&state=${state ? 1 : 0}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(err => {
          console.error(err);
          element.checked = !element.checked;
        });
    }

    window.onload = function() {
      loadStates();
      setInterval(loadStates, 5000);
    };
  </script>
</body>
</html>
)rawliteral";

  return html;
}

void setup() {
  Serial.begin(115200);

  // Set device as a Wi-Fi Station and establish a Wi-Fi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  // Print the ESP32 IP Address
  Serial.print("Access ESP32 IP Address: http://");
  Serial.println(WiFi.localIP());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

  // Register peers
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Register first peer
  memcpy(peerInfo.peer_addr, board1Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register second peer 
  memcpy(peerInfo.peer_addr, board2Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Root URL handler
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", renderHTML());
  });

  // LED Control via ESP-NOW
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    // Validate URL parameters
    if (request->hasParam("board") && request->hasParam("gpio") && request->hasParam("state")) {
      int board  = request->getParam("board")->value().toInt();
      int gpio   = request->getParam("gpio")->value().toInt();
      bool newState = request->getParam("state")->value().toInt() == 1;

      bool* statePtr = getStatePtr(board, gpio);
      if (statePtr == nullptr) {
        request->send(400, "text/plain", "Invalid board/gpio");
        return;
      }

      // Set up the pending update for delivery tracking
      pendingUpdate.active   = true;
      pendingUpdate.board    = board;
      pendingUpdate.gpio     = gpio;
      pendingUpdate.newState = newState;
      pendingUpdate.oldState = *statePtr;

      struct_message msg;
      msg.gpio  = gpio;
      msg.state = newState;

      uint8_t* target = (board == 1) ? board1Address : board2Address;
      esp_err_t result = sendEspNowMessage(target, msg);

      if (result != ESP_OK) {
        // If sending failed, revert the GPIO state immediately
        pendingUpdate.active = false;
        request->send(500, "text/plain", "Send failed (not transmitted)");
      } else {
        // If sending succeeded, wait for delivery confirmation in the callback
        request->send(200, "text/plain", "Sent, awaiting delivery confirmation");
      }
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  // Get confirmed states of each GPIO by board
  server.on("/states", HTTP_GET, [](AsyncWebServerRequest *request){
    // Return the confirmed states of each GPIO by board in JSON format
    String json = "{";
    json += "\"b1Btn1\":" + String(board1Btn1 ? "true" : "false") + ",";
    json += "\"b1Btn2\":" + String(board1Btn2 ? "true" : "false") + ",";
    json += "\"b2Btn1\":" + String(board2Btn1 ? "true" : "false") + ",";
    json += "\"b2Btn2\":" + String(board2Btn2 ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
  });
  
  server.begin();
  Serial.println("ESP32 Web Server is Ready!");
}

void loop() {
  
}
