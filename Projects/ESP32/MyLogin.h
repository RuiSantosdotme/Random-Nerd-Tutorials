/*********
  Created by Ron Brinkman
  Complete instructions at https://RandomNerdTutorials.com/esp32-save-credentials-separate-file/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// Keep account names, passwords, tokens, etc. current in this file
// When visiting another location put their credentials in the "currently in effect" section

// Credentials currently in effect
#define STASSID               "REPLACE_WITH_YOUR_SSID"                   // WiFi
#define STAPSK                "REPLACE_WITH_YOUR_PASSWORD"

#define emailSenderAccount    "YOUR_SENDER_EMAIL@gmail.com"      // Email
#define emailSenderPassword   "YOUR_EMAIL_APP_PASSWORD"
#define emailRecipient        "YOUR_EMAIL_RECIPIENT@example.com"

#define DuckDNStoken          "YOUR_DUCKDNS_TOKEN"               // DuckDNS

/*********Saved credentials for substitution above when "on the road"
//Credentials used at home
#define STASSID               "my_wifi_ssid"
#define STAPSK                "my_wifi_password"
#define emailSenderAccount    "my_sender_email@gmail.com"
#define emailSenderPassword   "my_email_app_password"
#define emailRecipient        "my_email_recipient@example.com"
#define DuckDNStoken          "my_duckdns_token"

// Credentials used at Mom and Dad's house
#define STASSID               "parents_wifi_ssid"
#define STAPSK                "parents_wifi_password"

// Credentials used at wife's Mom and Dad's house
#define STASSID               "wifes_parents_wifi_ssid"
#define STAPSK                "wifes_parents_wifi_password"

// Credentials used at our daughter's house
#define STASSID               "daughter_wifi_ssid"
#define STAPSK                "daughter_wifi_password"

// Credentials used at our son's house
#define STASSID               "son_wifi_ssid"
#define STAPSK                "son_wifi_password"

// Credentials used at our friends Joe and Sally's house
#define STASSID               "friend_wifi_ssid"
#define STAPSK                "friend_wifi_password"
*********/
