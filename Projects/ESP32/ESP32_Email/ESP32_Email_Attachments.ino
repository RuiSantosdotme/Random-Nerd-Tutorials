/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-send-email-smtp-server-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include "ESP32_MailClient.h"
#include "SD.h"
#include "SPIFFS.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// To send Emails using Gmail on port 465 (SSL), you need to create an app password: https://support.google.com/accounts/answer/185833
#define emailSenderAccount    "EXAMPLE_EMAIL@gmail.com"
#define emailSenderPassword   "YOUR_EXAMPLE_EMAIL_PASSWORD"
#define emailRecipient        "YOUR_EMAIL_RECIPIENT@EXAMPLE.com"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "ESP32 Test Email with Attachments"

// The Email Sending data object contains config and data to send
SMTPData smtpData;

// Callback function to get the Email sending status
void sendCallback(SendStatus info);

void setup(){
  Serial.begin(115200);
  Serial.println();
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // Or initialize SD Card (comment the preceding SPIFFS begin)
  /*
  MailClient.sdBegin(14, 2, 15, 13); // (SCK, MISO, MOSI, SS)
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return "";
  }*/
  
  Serial.print("Connecting");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.println();
  Serial.println("Preparing to send email");
  Serial.println();
  
  // Set the SMTP Server Email host, port, account and password
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);

  // For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be 
  // enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
  //smtpData.setSTARTTLS(true);

  // Set the sender name and Email
  smtpData.setSender("ESP32", emailSenderAccount);

  // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");

  // Set the subject
  smtpData.setSubject(emailSubject);

  // Set the message with HTML format
  smtpData.setMessage("<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP32 board</p></div>", true);
  // Set the email message in text format (raw)
  //smtpData.setMessage("Hello World! - Sent from ESP32 board", false);

  // Add recipients, you can add more than one recipient
  smtpData.addRecipient(emailRecipient);
  //smtpData.addRecipient("YOUR_RECIPIENT_EMAIL_ADDRESS@example.com");

  // Add attach files from SD card or SPIFFS
  // Comment the next two lines, if no SPIFFS files created or SD card connected
  smtpData.addAttachFile("/ESP32-CAM-Test.jpg", "image/jpg");
  smtpData.addAttachFile("/text_file.txt");

  // Add some custom header to message
  // See https://tools.ietf.org/html/rfc822
  // These header fields can be read from raw or source of message when it received)
  //smtpData.addCustomMessageHeader("Date: Sat, 10 Aug 2019 21:39:56 -0700 (PDT)");
  // Be careful when set Message-ID, it should be unique, otherwise message will not store
  //smtpData.addCustomMessageHeader("Message-ID: <abcde.fghij@gmail.com>");

  // Set the storage type to attach files in your email (SPIFFS or SD Card)
  smtpData.setFileStorageType(MailClientStorageType::SPIFFS);
  //smtpData.setFileStorageType(MailClientStorageType::SD);

  smtpData.setSendCallback(sendCallback);

  //Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

  //Clear all data from Email object to free memory
  smtpData.empty();
}

void loop() {
  
}

// Callback function to get the Email sending status
void sendCallback(SendStatus msg) {
  // Print the current status
  Serial.println(msg.info());

  // Do something when complete
  if (msg.success()) {
    Serial.println("----------------");
  }
}
