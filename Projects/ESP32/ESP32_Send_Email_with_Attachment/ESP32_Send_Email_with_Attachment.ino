/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at: https://RandomNerdTutorials.com/esp32-send-email-smtp-server-arduino-ide/  
*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FS.h>
#include <LittleFS.h>

#define ENABLE_SMTP  // Allows SMTP class and data
#define ENABLE_DEBUG // Allows debugging
#define ENABLE_FS // Allow filesystem integration
#define MY_FS LittleFS

#include <ReadyMail.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Sender SMTP settings (GMAIL)
// Change if using a different provider
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

// Sender email, app password, and name
#define AUTHOR_EMAIL "SENDER_EMAIL@XXX.com"
#define AUTHOR_APP_PASS "EMAIL_APP_PASSWORD"
#define AUTHOR_NAME "ESP32"

//Recipient's email
#define RECIPIENT_EMAIL "RECIPIENTE_EMAIL@XXXX.com"
#define RECIPIENT_NAME "RECIPIENT_NAME"

WiFiClientSecure ssl_client;
SMTPClient smtp(ssl_client);

// Create a file object
File myFile;

void fileCb(File &file, const char *filename, readymail_file_operating_mode mode) {
    switch (mode) {
    case readymail_file_mode_open_read:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_READ);
        break;
    case readymail_file_mode_open_write:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_WRITE);
        break;
    case readymail_file_mode_open_append:
        myFile = MY_FS.open(filename, FILE_OPEN_MODE_APPEND);
        break;
    case readymail_file_mode_remove:
        MY_FS.remove(filename);
        break;
    default:
        break;
    }

    // This is required by library to get the file object
    // that uses in its read/write processes.
    file = myFile;
}

void initLittleFS() {
    if (!MY_FS.begin(true)){
        Serial.println("LittleFS Mount Failed");
        return;
    }
}

// For more information, see http://bit.ly/474niML
void smtpCb(SMTPStatus status){
    if (status.progress.available)
        ReadyMail.printf("ReadyMail[smtp][%d] Uploading file %s, %d %% completed\n", status.state,
                         status.progress.filename.c_str(), status.progress.value);
    else
        ReadyMail.printf("ReadyMail[smtp][%d]%s\n", status.state, status.text.c_str());
}

void addFileAttachment(SMTPMessage &msg, const String &filename, const String &mime, const String &name, FileCallback cb, const String &filepath, const String &encoding = "", const String &cid = "") {
    Attachment attachment;
    attachment.filename = filename;
    attachment.mime = mime;
    attachment.name = name;
    // The inline content disposition.
    // Should be matched the image src's cid in html body
    attachment.content_id = cid;
    attachment.attach_file.callback = cb;
    attachment.attach_file.path = filepath;
    // Specify only when content is already encoded.
    attachment.content_encoding = encoding;
    msg.attachments.add(attachment, cid.length() > 0 ? attach_type_inline : attach_type_attachment);
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    initLittleFS();

    // If server SSL certificate verification was ignored for this ESP32 WiFiClientSecure.
    // To verify root CA or server SSL cerificate,
    // please consult your SSL client documentation.
    ssl_client.setInsecure();

    smtp.connect(SMTP_HOST, SMTP_PORT, smtpCb);
    if (!smtp.isConnected())
        return;

    smtp.authenticate(AUTHOR_EMAIL, AUTHOR_APP_PASS, readymail_auth_password);
    if (!smtp.isAuthenticated())
        return;

    SMTPMessage msg;
    msg.headers.add(rfc822_from, String(AUTHOR_NAME) + " <" + AUTHOR_EMAIL + ">");
    msg.headers.add(rfc822_to, String(RECIPIENT_NAME) + " <" + RECIPIENT_EMAIL + ">");
    msg.headers.add(rfc822_subject, "ESP32 Attachments");

    String bodyText = "Hello! See attachments";
    msg.text.body(bodyText);
    msg.html.body("<html><body><div style=\"color:#cc0066;\">" + bodyText + "</div></body></html>");

    // Set NTP config time
    /* For times east of the Prime Meridian use 0-12
    For times west of the Prime Meridian add 12 to the offset.
    Ex. American/Denver GMT would be -6. 6 + 12 = 18 */
    const int gmtOffset_sec = 0; //offset time in seconds
    const int daylightOffset_sec = 0; //daylight saving time offset in seconds

    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
    // Set timestamp for the email
    while (time(nullptr) < 100000) delay(100);
    msg.timestamp = time(nullptr);

    addFileAttachment(msg, "image.png", "image/png", "image.png", fileCb, "/image.png");
    addFileAttachment(msg, "text_file.txt", "text/plain", "text_file.txt", fileCb, "/text_file.txt");
    Serial.print("Sending email");
    smtp.send(msg);
}

void loop(){
    
}