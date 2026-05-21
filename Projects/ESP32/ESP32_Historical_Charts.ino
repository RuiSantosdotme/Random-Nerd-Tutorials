/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-charts-historical-data/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SD.h>
#include <SPI.h>
#include <time.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// BME280 GPIOs
#define SDA_PIN 21
#define SCL_PIN 22

// MicroSD Card Module GPIOs
#define SD_CS   5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCK  18

// NTP server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;      // Change to your timezone
const int daylightOffset_sec = 0;

// Timer to take new readings
unsigned long previousMillis = 0;
const long interval = 60000;  // 60 seconds

// CSV filename
const char* dataFile = "/bme280_log.csv";

// AsyncWebServer Web Server
AsyncWebServer server(80);
// BME280
Adafruit_BME280 bme;

// Get Time
String getTimeStr() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "00:00:00";
  char buf[9]; sprintf(buf, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  return String(buf);
}

// Get Date
String getDateStr() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "1970-01-01";
  char buf[11]; sprintf(buf, "%04d-%02d-%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday);
  return String(buf);
}

// Initialize MicroSD Card and create the .csv file
void initSD() {
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) { Serial.println("SD Card Failed"); return; }
  
  if (!SD.exists(dataFile)) {
    File f = SD.open(dataFile, FILE_WRITE);
    if (f) { f.println("temp_c,temp_f,humidity,pressure,time,day"); f.close(); }
  }
}

// Log BME280 data on the MicroSD Card
void logData() {
  float tempC = bme.readTemperature();
  float tempF = tempC * 9.0 / 5.0 + 32.0;
  float hum = bme.readHumidity();
  float press = bme.readPressure() / 100.0F;

  File file = SD.open(dataFile, FILE_APPEND);
  if (file) {
    file.printf("%.2f,%.2f,%.2f,%.2f,%s,%s\n", 
                tempC, tempF, hum, press, getTimeStr().c_str(), getDateStr().c_str());
    file.close();
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize BME280 Sensor
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!bme.begin(0x76)) { 
    Serial.println("BME280 not found!");
    while(1); 
  }

  // Start the Wi-Fi connection
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

  // Configure NTP time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(2000);

  // Initialize MicroSD Card
  initSD();

  // Root URL handler for the mains HTML web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Charts with Data Logger</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=yes">
  <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.1/dist/chart.umd.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/hammerjs@2.0.8/hammer.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-zoom@2.0.1/dist/chartjs-plugin-zoom.min.js"></script>
  <style>
    body {font-family: Arial, sans-serif; background:#f4f4f4; margin:0; padding:15px;}
    .container {max-width:1300px; margin:auto; background:white; padding:20px; border-radius:10px; box-shadow:0 4px 15px rgba(0,0,0,0.1);}
    h1 {text-align:center;}
    .controls { text-align:center; margin:20px 0; display:flex; flex-wrap:wrap; justify-content:center; gap:12px; }
    .btn { padding:12px 22px; font-size:16px; font-weight:600; border:none; border-radius:8px; cursor:pointer; transition:all 0.2s ease; box-shadow:0 2px 6px rgba(0,0,0,0.1); }
    .btn:hover { transform:translateY(-2px); box-shadow:0 4px 12px rgba(0,0,0,0.15); }
    .btn-blue { background:#2196F3; color:white; }
    .btn-green { background:#4CAF50; color:white; }
    .btn-red { background:#f44336; color:white; }
    .btn-gray { background:#6c777d; color:white; }
    .chart-container {position:relative; height:380px; margin-bottom:40px; border:1px solid #ddd; border-radius:8px; padding:15px; touch-action: none;}
    .chart-header {display:flex; justify-content:space-between; align-items:center; margin-bottom:12px; flex-wrap:wrap; gap:10px;}
    .toggle-group {display:inline-flex; background:#f1f1f1; border-radius:50px; padding:4px; box-shadow:inset 0 2px 4px rgba(0,0,0,0.1);}
    .toggle-btn {padding:8px 24px; font-size:15px; font-weight:600; border:none; border-radius:50px; cursor:pointer; transition:all 0.3s;}
    .toggle-btn.active {background:#393b41; color:white; box-shadow:0 2px 6px rgba(33,150,243,0.3);}
    .latest-reading { background:#f8f8f8; padding:15px; font-size:16px; border-radius:5px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32 Charts with Data Logger</h1>
    <p style="text-align:center;">New readings are added to your chart automatically, you don't need to refresh the web page.</p>
    <div class="controls">
      <button class="btn btn-blue" onclick="loadCSVData()">Refresh Charts</button>
      <button class="btn btn-green" onclick="downloadCSV()">Download CSV</button>
      <button class="btn btn-red" onclick="deleteData()">Delete All Data</button>
      <button class="btn btn-gray" onclick="resetZoomAll()">Reset All Zoom</button>
    </div>
    <div class="chart-header">
      <h2>Temperature</h2>
      <div class="toggle-group">
        <button id="btnC" class="toggle-btn active" onclick="setUnit('C')">&deg;C</button>
        <button id="btnF" class="toggle-btn" onclick="setUnit('F')">&deg;F</button>
      </div>
    </div>
    <div class="chart-container"><canvas id="tempChart"></canvas></div>
    <h2>Humidity</h2>
    <div class="chart-container"><canvas id="humChart"></canvas></div>
    <h2>Pressure</h2>
    <div class="chart-container"><canvas id="pressChart"></canvas></div>
    <h3>Latest Reading</h3>
    <pre id="latest" class="latest-reading"></pre>
  </div>
  <script>
    let tempChart, humChart, pressChart;
    let allData = [];
    let currentUnit = 'C';
    function createCharts() {
      const zoomOptions = {
        zoom: { wheel: { enabled: true }, pinch: { enabled: true }, mode: 'x', drag: { enabled: true } },
        pan: { enabled: true, mode: 'x', threshold: 5 }
      };

      const makeChart = (canvasId, color, yLabel) => {
        const chart = new Chart(document.getElementById(canvasId), {
          type: 'line',
          data: { labels: [], datasets: [{ label: yLabel, borderColor: color, tension: 0.2, data: [] }] },
          options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: { mode: 'index', intersect: false },
            scales: {
              x: { title: { display: true, text: 'Timestamp' } },
              y: { title: { display: true, text: yLabel } }
            },
            plugins: { zoom: zoomOptions }
          }
        });

        // Double-click to reset zoom
        document.getElementById(canvasId).addEventListener('dblclick', () => {
          chart.resetZoom();
        });

        return chart;
      };

      tempChart = makeChart('tempChart', '#2ecc71', 'Temperature');
      humChart  = makeChart('humChart',  '#3498db', 'Humidity (%)');
      pressChart= makeChart('pressChart', '#8c479d', 'Pressure (hPa)');
    }
    async function loadCSVData() {
      try {
        const response = await fetch('/download');
        const csvText = await response.text();
        allData = parseCSV(csvText);

        if (allData.length === 0) {
          document.getElementById('latest').textContent = "No data logged yet.";
          return;
        }

        const last = allData[allData.length-1];
        document.getElementById('latest').innerHTML = 
          `Temperature: ${last.temp_c}°C / ${last.temp_f}°F<br>` +
          `Humidity: ${last.hum}%<br>` +
          `Pressure: ${last.press} hPa<br>` +
          `<strong>${last.day} ${last.time}</strong>`;

        const displayData = allData.slice(-1200);
        const labels = displayData.map(d => d.day + " " + d.time);

        const tempValues = displayData.map(d => currentUnit === 'C' ? parseFloat(d.temp_c) : parseFloat(d.temp_f));

        tempChart.data.labels = labels;
        tempChart.data.datasets[0].label = `Temperature (°${currentUnit})`;
        tempChart.data.datasets[0].data = tempValues;
        tempChart.update();

        humChart.data.labels = labels;
        humChart.data.datasets[0].data = displayData.map(d => parseFloat(d.hum));
        humChart.update();

        pressChart.data.labels = labels;
        pressChart.data.datasets[0].data = displayData.map(d => parseFloat(d.press));
        pressChart.update();

      } catch(e) { console.error("Error:", e); }
    }
    function parseCSV(csv) {
      const lines = csv.trim().split('\n');
      const result = [];
      for (let i = 1; i < lines.length; i++) {
        if (!lines[i].trim()) continue;
        const [temp_c, temp_f, hum, press, time, day] = lines[i].split(',');
        result.push({temp_c, temp_f, hum, press, time, day});
      }
      return result;
    }
    function setUnit(unit) {
      currentUnit = unit;
      document.getElementById('btnC').classList.toggle('active', unit === 'C');
      document.getElementById('btnF').classList.toggle('active', unit === 'F');
      if (allData.length > 0) loadCSVData();
    }
    function downloadCSV() { 
      window.location.href = '/download';
    }
    function resetZoomAll() {
      tempChart.resetZoom();
      humChart.resetZoom();
      pressChart.resetZoom();
    }
    async function deleteData() {
      if (!confirm("Delete all logged data?")) return;
      await fetch('/delete', {method: 'POST'});
      alert("Data deleted");
      loadCSVData();
    }
    window.onload = () => {
      createCharts();
      setUnit('C');
      loadCSVData();
      setInterval(loadCSVData, 30000);
    };
  </script>
</body>
</html>
)rawliteral";
    request->send(200, "text/html; charset=UTF-8", html);
  });

  // Download .csv file stored on MicroSD Card
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, dataFile, "text/csv", true);
  });

  // Delete .csv file stored on MicroSD Card
  server.on("/delete", HTTP_POST, [](AsyncWebServerRequest *request){
    if (SD.exists(dataFile)) SD.remove(dataFile);
    File f = SD.open(dataFile, FILE_WRITE);
    if (f) { f.println("temp_c,temp_f,humidity,pressure,time,day"); f.close(); }
    request->send(200);
  });

  server.begin();
  Serial.println("ESP32 Web Server: Charts with Historical Data Ready!");
}

void loop() {
  // Timer
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    logData();
  }
  delay(10);
}
