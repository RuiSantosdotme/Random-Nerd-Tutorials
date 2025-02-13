/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/lilygo-ttgo-t-a7670g-a7670e-a7670sa-esp32/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
// set SIM card pin, if any
#define SIM_PIN ""

// It depends on the operator whether you need to set up an APN or not. 
// With some operators if you do not set up an APN, the connection will be rejected when registering for the network.
#define NETWORK_APN "REPLACE_WITH_YOUR_NETWORK_APN"

// If the status code 715 is returned, you might need to update your SIMCOM firmware please see here: https://github.com/Xinyuan-LilyGO/LilyGO-T-A76XX/issues/117
// How to update the firmware: https://github.com/Xinyuan-LilyGO/LilyGO-T-A76XX/blob/main/docs/update_fw.md
// Some websites block the ESP32 connection, so it might return an error.
// The githubusercontent.com URL defined below has been tested and it returns the proper HTTPS GET request
const char *request_url[] = {
  "https://gist.githubusercontent.com/RuiSantosdotme/7db8537cef1c84277c268c76a58d07ff/raw/d3fe4cd6eff1ed43e6dbd1883ab7eba8414e2406/gistfile1.txt"
};

#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define LILYGO_T_A7670

#if defined(LILYGO_T_A7670)
  #define MODEM_BAUDRATE                      (115200)
  #define MODEM_DTR_PIN                       (25)
  #define MODEM_TX_PIN                        (26)
  #define MODEM_RX_PIN                        (27)
  // The modem boot pin needs to follow the startup sequence.
  #define BOARD_PWRKEY_PIN                    (4)
  #define BOARD_ADC_PIN                       (35)
  // The modem power switch must be set to HIGH for the modem to supply power.
  #define BOARD_POWERON_PIN                   (12)
  #define MODEM_RING_PIN                      (33)
  #define MODEM_RESET_PIN                     (5)
  #define BOARD_MISO_PIN                      (2)
  #define BOARD_MOSI_PIN                      (15)
  #define BOARD_SCK_PIN                       (14)
  #define BOARD_SD_CS_PIN                     (13)
  #define BOARD_BAT_ADC_PIN                   (35)
  #define MODEM_RESET_LEVEL                   HIGH
  #define SerialAT                            Serial1
  #define MODEM_GPS_ENABLE_GPIO               (-1)
  #define MODEM_GPS_ENABLE_LEVEL              (-1)
  #ifndef TINY_GSM_MODEM_A7670
    #define TINY_GSM_MODEM_A7670
  #endif
#endif

// YOU MUST USE THE TinyGSM-fork LIBRARY BY lewisxhe - https://github.com/lewisxhe/TinyGSM-fork
// If you already have the vshymanskyy/TinyGSM library installed, you must remove it and use the TinyGSM-fork version by lewisxhe mentioned above
#include <TinyGsmClient.h>

// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS
#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, Serial);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

void HTTPSGetRequest() {
  for (int i = 0; i < sizeof(request_url) / sizeof(request_url[0]); ++i) {
    int retry = 3;
    while (retry--) {
      Serial.print("Request URL : ");
      Serial.println(request_url[i]);

      // Set GET URT
      if (!modem.https_set_url(request_url[i])) {
        Serial.print("Failed to request : "); Serial.println(request_url[i]);
        // Debug
        // modem.sendAT("+CSSLCFG=\"enableSNI\",0,1");
        // modem.waitResponse();
        delay(3000);
        continue;
      }

      // Send GET request
      int httpCode = 0;
      httpCode = modem.https_get();
      if (httpCode != 200) {
        Serial.print("HTTP get failed ! error code = ");
        Serial.println(httpCode);
        delay(3000);
        continue;
      }

      // Get HTTPS header information
      String header = modem.https_header();
      Serial.print("HTTP Header : ");
      Serial.println(header);
      delay(1000);

      // Get HTTPS response
      String body = modem.https_body();
      Serial.print("HTTP body : ");
      Serial.println(body);
      delay(3000);
      break;
    }
    Serial.println("-------------------------------------");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Init...");
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

  pinMode(BOARD_POWERON_PIN, OUTPUT);
  digitalWrite(BOARD_POWERON_PIN, HIGH);

  // Set modem reset pin, reset modem
  pinMode(MODEM_RESET_PIN, OUTPUT);
  digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL); delay(100);
  digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL); delay(2600);
  digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);

  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, HIGH);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);

  // Check if the modem is online
  Serial.println("Start modem...");

  int retry = 0;
  while (!modem.testAT(1000)) {
    Serial.print(".");
    if (retry++ > 10) {
      digitalWrite(BOARD_PWRKEY_PIN, LOW);
      delay(100);
      digitalWrite(BOARD_PWRKEY_PIN, HIGH);
      delay(1000);
      digitalWrite(BOARD_PWRKEY_PIN, LOW);
      retry = 0;
    }
  }
  Serial.println();

  // Check if SIM card is online
  SimStatus sim = SIM_ERROR;
  while (sim != SIM_READY) {
    sim = modem.getSimStatus();
    switch (sim) {
      case SIM_READY:
        Serial.println("SIM card online");
        break;
      case SIM_LOCKED:
        Serial.println("The SIM card is locked. Please unlock the SIM card first.");
        modem.simUnlock(SIM_PIN);
        break;
      default:
        break;
    }
    delay(1000);
  }

  if (!modem.setNetworkMode(MODEM_NETWORK_AUTO)) {
    Serial.println("Set network mode failed!");
  }
  String mode = modem.getNetworkModes();
  Serial.print("Current network mode : ");
  Serial.println(mode);

#ifdef NETWORK_APN
  Serial.printf("Set network apn : %s\n", NETWORK_APN);
  modem.sendAT(GF("+CGDCONT=1,\"IP\",\""), NETWORK_APN, "\"");
  if (modem.waitResponse() != 1) {
    Serial.println("Set network apn error !");
  }
#endif

  // Check network registration status and network signal status
  int16_t sq ;
  Serial.print("Wait for the modem to register with the network.");
  RegStatus status = REG_NO_RESULT;
  while (status == REG_NO_RESULT || status == REG_SEARCHING || status == REG_UNREGISTERED) {
    status = modem.getRegistrationStatus();
    switch (status) {
      case REG_UNREGISTERED:
      case REG_SEARCHING:
        sq = modem.getSignalQuality();
        Serial.printf("[%lu] Signal Quality:%d\n", millis() / 1000, sq);
        delay(1000);
        break;
      case REG_DENIED:
        Serial.println("Network registration was rejected, please check if the APN is correct");
        return ;
      case REG_OK_HOME:
        Serial.println("Online registration successful");
        break;
      case REG_OK_ROAMING:
        Serial.println("Network registration successful, currently in roaming mode");
        break;
      default:
        Serial.printf("Registration Status:%d\n", status);
        delay(1000);
        break;
    }
  }
  Serial.println();

  Serial.printf("Registration Status:%d\n", status);
  delay(1000);

  String ueInfo;
  if (modem.getSystemInformation(ueInfo)) {
    Serial.print("Inquiring UE system information:");
    Serial.println(ueInfo);
  }

  if (!modem.setNetworkActive()) {
    Serial.println("Enable network failed!");
  }
  delay(5000);

  String ipAddress = modem.getLocalIP();
  Serial.print("Network IP:"); Serial.println(ipAddress);

  // Initialize HTTPS
  modem.https_begin();

  HTTPSGetRequest();
}

void loop() {
  // Debug AT
  if (SerialAT.available()) {
    Serial.write(SerialAT.read());
  }
  if (Serial.available()) {
    SerialAT.write(Serial.read());
  }
  delay(1);
}

#ifndef TINY_GSM_FORK_LIBRARY
  #error "The correct library was NOT found. You must install TinyGSM-fork by lewisxhe - https://github.com/lewisxhe/TinyGSM-fork - No correct definition detected, Please copy all the [lib directories](https://github.com/Xinyuan-LilyGO/LilyGO-T-A76XX/tree/main/lib) to the arduino libraries directory , See README"
#endif
