/* Example showing timestamp support in LittleFS - Released into the public domain.
   Earle F. Philhower, III <earlephilhower@yahoo.com> Official example: https://github.com/esp8266/Arduino/blob/master/libraries/LittleFS/examples/LittleFS_Timestamp/LittleFS_Timestamp.ino
*/

// Project details: https://RandomNerdTutorials.com/esp8266-nodemcu-write-data-littlefs-arduino/

#include <FS.h>
#include <LittleFS.h>

int mydata;

void readFile(const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) { Serial.write(file.read()); }
  file.close();
}

void writeFile(const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  delay(2000);  // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void appendFile(const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void setup() {
  Serial.begin(115200);
  
  Serial.println("Mount LittleFS");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  else{
    Serial.println("Little FS Mounted Successfully");
  }
   // Check if the file already exists to prevent overwritting existing data
   bool fileexists = LittleFS.exists("/data.txt");
   Serial.print(fileexists);
   if(!fileexists) {
       Serial.println("File doesn’t exist");
       Serial.println("Creating file...");
       // Create File and add header
       writeFile("/data.txt", "MY ESP8266 DATA \r\n");
   }
   else {
       Serial.println("File already exists");
   }
}

void loop() {
  mydata = random (0, 1000);
  appendFile("/data.txt", (String(mydata)+ "\r\n").c_str()); //Append data to the file
  readFile("/data.txt"); // Read the contents of the file
  delay(30000);
}
