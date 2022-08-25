# Original Source: https://github.com/A223D/detaBaseArduinoESP32/blob/main/examples/detaLibTest/detaLibTest.ino

#include <detaBaseArduinoESP32.h>
#include <WiFiClientSecure.h>
#define LED 2

char* apiKey = "MY_KEY";
char* detaID = "MY_ID";
char* detaBaseName = "MY_BASE";

WiFiClientSecure client;
DetaBaseObject detaObj(client, detaID, detaBaseName, apiKey, true);


void setup() {
  Serial.begin(115200);
  Serial.println("Let's begin initialization");
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.println("Reached before WiFi init");
  WiFi.begin("0xCAFE", "0xC0FFEE");
  Serial.println("Waiting to connect to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  digitalWrite(LED, HIGH);

}

// PUT "{\"items\":[{\"age\":4}]}"

//INSERT "{\"item\":{\"key\":\"cba\",\"age\":4}}"

//INSERT "{\"item\":{\"key\":\"abc\",\"age\":4}}"

//UPDATE "{\"increment\":{\"age\":1}}", key:abc

//UPDATE "{\"increment\":{\"age\":1}}", key:cba

//QUERY "{\"query\":[{\"age?lt\": 10}]}"

void loop() {
  printResult(detaObj.putObject("{\"items\":[{\"age\":4}]}"));
  Serial.println();
  printResult(detaObj.getObject("cba"));
  Serial.println();
  printResult(detaObj.deleteObject("abc"));
  Serial.println();
  printResult(detaObj.insertObject("{\"item\":{\"key\":\"cba\",\"age\":4}}"));
  Serial.println();
  printResult(detaObj.insertObject("{\"item\":{\"key\":\"abc\",\"age\":4}}"));
  Serial.println();
  printResult(detaObj.updateObject("{\"increment\":{\"age\":1}}", "abc"));
  Serial.println();
  printResult(detaObj.updateObject("{\"increment\":{\"age\":1}}", "bcs"));
  Serial.println();
  printResult(detaObj.query("{\"query\":[{\"age?lt\": 10}]}"));
  Serial.println();

  while (true);
}
