/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace with your SSID, password and MQTT broker IP address
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
const char* mqtt_server = "REPLACE_WITH_YOUR_MQTT_BROKER_IP";
//For example
//const char* mqtt_server = "192.168.1.144";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  while (!client.connected()) {
    reconnect();
  }
  Serial.print("Motion Detected");
  // Publish MQTT message
  String mqttMessage = "Motion Detected";  
  client.publish("esp/pir", mqttMessage.c_str());
}

void loop() {
  client.loop();
}
