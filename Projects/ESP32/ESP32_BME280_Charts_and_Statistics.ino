/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-charts-statistics/
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
  <title>ESP32 BME280: Charts & Stats</title>
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
    .stats-section { margin: 30px 0; }
    .stats-section h2 { margin-bottom: 15px; }
    .stats-grid { display:grid; grid-template-columns: repeat(auto-fit, minmax(190px, 1fr)); gap:14px; margin-bottom:22px; }
    .stat-card { background:#f8f8f8; border:1px solid #e2e2e2; border-radius:8px; padding:14px 16px; }
    .stat-card .label { font-size:13px; color:#666; margin-bottom:6px; font-weight:600; text-transform:uppercase; letter-spacing:0.3px; }
    .stat-card .value { font-size:22px; font-weight:700; color:#222; }
    .stat-card .subvalue { font-size:12.5px; color:#888; margin-top:3px; }
    .stat-card.warn .value { color:#c0392b; }
    .stat-card.cool .value { color:#2980b9; }
    .stat-card.mild .value { color:#27ae60; }
    .subsection-title { font-size:16px; font-weight:700; margin: 22px 0 10px; color:#333; border-left:4px solid #2196F3; padding-left:10px; }
    table.swing-table { width:100%; border-collapse:collapse; font-size:16px; margin-top:8px; }
    table.swing-table th, table.swing-table td { text-align:left; padding:8px 10px; border-bottom:1px solid #eee; }
    table.swing-table th { background:#f1f1f1; font-weight:700; }
    table.swing-table tr:last-child td { border-bottom:none; }
    table.swing-table td.num { text-align:right; font-variant-numeric: tabular-nums; }
    .no-data-msg { color:#888; font-style:italic; padding:10px 0; }
    .info-note { font-size:12.5px; color:#999; margin-top:6px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32 BME280: Charts & Stats</h1>
    <div class="info-note" style="text-align:center;">New readings are added to your chart automatically, you don't need to refresh the web page.</div>
    <div class="controls">
      <button class="btn btn-blue" onclick="loadCSVData()">Refresh Charts</button>
      <button class="btn btn-green" onclick="downloadCSV()">Download CSV</button>
      <button class="btn btn-red" onclick="deleteData()">Delete All Data</button>
      <button class="btn btn-gray" onclick="resetZoomAll()">Reset All Zoom</button>
    </div>
    <div class="chart-header">
      <h2>Temperature</h2>
      <div class="toggle-group">
        <button class="toggle-btn unit-btn active" data-unit="C" onclick="setUnit('C')">&deg;C</button>
        <button class="toggle-btn unit-btn" data-unit="F" onclick="setUnit('F')">&deg;F</button>
      </div>
    </div>
    <div class="chart-container"><canvas id="tempChart"></canvas></div>
    <h2>Humidity</h2>
    <div class="chart-container"><canvas id="humChart"></canvas></div>
    <h2>Pressure</h2>
    <div class="chart-container"><canvas id="pressChart"></canvas></div>
    <h3>Latest Reading</h3>
    <div class="stats-grid">
      <div class="stat-card warn"><div class="label">Temperature</div><div class="value" id="latestTemp">--</div></div>
      <div class="stat-card cool"><div class="label">Humidity</div><div class="value" id="latestHum">--</div></div>
      <div class="stat-card mild"><div class="label">Pressure</div><div class="value" id="latestPress">--</div></div>
      <div class="stat-card"><div class="label">Timestamp</div><div class="value" id="latestTime">--</div></div>
    </div>
    <div id="latestEmpty" class="no-data-msg" style="display:none;">No data logged yet.</div>
    <div class="stats-section">
      <div class="chart-header">
        <h2>Temperature Statistics</h2>
        <div class="toggle-group">
          <button class="toggle-btn unit-btn active" data-unit="C" onclick="setUnit('C')">&deg;C</button>
          <button class="toggle-btn unit-btn" data-unit="F" onclick="setUnit('F')">&deg;F</button>
        </div>
      </div>
      <div id="statsEmpty" class="no-data-msg" style="display:none;">Not enough data yet to compute statistics.</div>
      <div id="statsContent">
        <div class="subsection-title">Last 24 Hours</div>
        <div class="stats-grid">
          <div class="stat-card warn"><div class="label">Max Temp (24h)</div><div class="value" id="stat24Max">--</div><div class="subvalue" id="stat24MaxTime"></div></div>
          <div class="stat-card cool"><div class="label">Min Temp (24h)</div><div class="value" id="stat24Min">--</div><div class="subvalue" id="stat24MinTime"></div></div>
        </div>
        <div class="subsection-title">Last 7 Days</div>
        <div class="stats-grid">
          <div class="stat-card warn"><div class="label">Max Temp (7d)</div><div class="value" id="stat7Max">--</div><div class="subvalue" id="stat7MaxTime"></div></div>
          <div class="stat-card cool"><div class="label">Min Temp (7d)</div><div class="value" id="stat7Min">--</div><div class="subvalue" id="stat7MinTime"></div></div>
        </div>
        <div class="subsection-title">Time Above / Below Thresholds (Last 7 Days)</div>
        <div class="stats-grid">
          <div class="stat-card warn"><div class="label" id="labelAboveThreshold">Above 25&deg;C</div><div class="value" id="statAbove25">--</div></div>
          <div class="stat-card mild"><div class="label" id="labelMidThreshold">15&ndash;25&deg;C</div><div class="value" id="statMid1525">--</div></div>
          <div class="stat-card cool"><div class="label" id="labelBelowThreshold">Below 15&deg;C</div><div class="value" id="statBelow15">--</div></div>
        </div>
        <div class="info-note">Estimated by adding up the time spent in each temperature band between consecutive readings. Gaps without readings longer than 15 minutes are ignored.</div>
        <div class="subsection-title">Time to Max / Time to Min</div>
        <div class="stats-grid">
          <div class="stat-card"><div class="label">Latest: Min &rarr; Max</div><div class="value" id="statMaxToday">--</div></div>
          <div class="stat-card"><div class="label">Latest: Max &rarr; Min</div><div class="value" id="statMinToday">--</div></div>
          <div class="stat-card"><div class="label">7d Avg: Min &rarr; Max</div><div class="value" id="statMaxAvg7">--</div></div>
          <div class="stat-card"><div class="label">7d Avg: Max &rarr; Min</div><div class="value" id="statMinAvg7">--</div></div>
        </div>
        <div class="subsection-title">Daily Temperature Swing (Last 7 Days)</div>
        <div class="chart-container" style="height:auto;">
          <table class="swing-table" id="swingTable">
            <thead><tr><th>Date</th><th class="num">Min</th><th class="num">Max</th><th class="num">Swing</th></tr></thead>
            <tbody id="swingTableBody"></tbody>
          </table>
        </div>
        <div class="subsection-title">Diurnal Pattern (Average by Hour of Day)</div>
        <div class="chart-container" style="height:320px;"><canvas id="diurnalChart"></canvas></div>
        <div class="info-note">Average of all loaded readings, grouped by hour of day (0 to 23). Uses the full log history that is stored in the .csv file.</div>
      </div>
    </div>
  </div>
  <script>
    let tempChart, humChart, pressChart, diurnalChart;
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

        document.getElementById(canvasId).addEventListener('dblclick', () => {
          chart.resetZoom();
        });

        return chart;
      };

      tempChart = makeChart('tempChart', '#2ecc71', 'Temperature');
      humChart  = makeChart('humChart',  '#3498db', 'Humidity (%)');
      pressChart= makeChart('pressChart', '#8c479d', 'Pressure (hPa)');

      diurnalChart = new Chart(document.getElementById('diurnalChart'), {
        type: 'line',
        data: { labels: [], datasets: [{ label: 'Avg Temperature', borderColor: '#e67e22', backgroundColor: 'rgba(230,126,34,0.15)', fill: true, tension: 0.3, data: [] }] },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          interaction: { mode: 'index', intersect: false },
          scales: {
            x: { title: { display: true, text: 'Hour of Day (0-23)' } },
            y: { title: { display: true, text: 'Avg Temperature' } }
          }
        }
      });
    }

    async function loadCSVData() {
      try {
        const response = await fetch('/download');
        const csvText = await response.text();
        allData = parseCSV(csvText);

        if (allData.length === 0) {
          document.getElementById('latestEmpty').style.display = 'block';
          ['latestTemp','latestHum','latestPress','latestTime'].forEach(id => {
            document.getElementById(id).textContent = '--';
          });
          clearStats();
          return;
        }
        document.getElementById('latestEmpty').style.display = 'none';

        const last = allData[allData.length-1];
        document.getElementById('latestTemp').textContent = fmtTemp(last.temp_c);
        document.getElementById('latestHum').textContent = `${last.hum}%`;
        document.getElementById('latestPress').textContent = `${last.press} hPa`;
        document.getElementById('latestTime').textContent = `${last.day} ${last.time}`;

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

        updateStats();

      } catch(e) { console.error("Error:", e); }
    }

    function parseCSV(csv) {
      const lines = csv.trim().split('\n');
      const result = [];
      for (let i = 1; i < lines.length; i++) {
        if (!lines[i].trim()) continue;
        const [temp_c, temp_f, hum, press, time, day] = lines[i].split(',');
        if (temp_c === undefined || time === undefined || day === undefined) continue;
        const ts = new Date(`${day}T${time}`);
        result.push({
          temp_c: parseFloat(temp_c),
          temp_f: parseFloat(temp_f),
          hum: parseFloat(hum),
          press: parseFloat(press),
          time, day,
          ts: isNaN(ts.getTime()) ? null : ts
        });
      }
      result.sort((a, b) => {
        if (!a.ts || !b.ts) return 0;
        return a.ts - b.ts;
      });
      return result;
    }

    function setUnit(unit) {
      currentUnit = unit;
      document.querySelectorAll('.unit-btn').forEach(btn => {
        btn.classList.toggle('active', btn.dataset.unit === unit);
      });
      if (allData.length > 0) { loadCSVData(); }
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

    function fmtTemp(c) {
      if (c === null || c === undefined || isNaN(c)) return '--';
      return currentUnit === 'C' ? `${c.toFixed(1)}°C` : `${(c * 9/5 + 32).toFixed(1)}°F`;
    }

    function fmtTempDelta(c) {
      if (c === null || c === undefined || isNaN(c)) return '--';
      return currentUnit === 'C' ? `Δ${c.toFixed(1)}°C` : `Δ${(c * 9/5).toFixed(1)}°F`;
    }

    function fmtDuration(mins) {
      if (mins === null || mins === undefined || isNaN(mins)) return '--';
      mins = Math.round(mins);
      const h = Math.floor(mins / 60);
      const m = mins % 60;
      if (h === 0) return `${m}m`;
      return `${h}h ${m}m`;
    }

    function fmtHM(mins) {
      if (mins === null || mins === undefined || isNaN(mins)) return '--';
      mins = Math.round(mins);
      const h = Math.floor(mins / 60);
      const m = mins % 60;
      return `${h}h ${m}m`;
    }

    function clearStats() {
      document.getElementById('statsContent').style.display = 'none';
      document.getElementById('statsEmpty').style.display = 'block';
    }

    function showStats() {
      document.getElementById('statsContent').style.display = 'block';
      document.getElementById('statsEmpty').style.display = 'none';
    }

    function validReadings() {
      return allData.filter(d => d.ts && !isNaN(d.temp_c));
    }

    function groupByDay(readings) {
      const map = new Map();
      for (const r of readings) {
        if (!map.has(r.day)) map.set(r.day, []);
        map.get(r.day).push(r);
      }
      const keys = Array.from(map.keys()).sort();
      return keys.map(k => ({ dateKey: k, readings: map.get(k) }));
    }

    function minMax(readings) {
      if (readings.length === 0) return null;
      let minR = readings[0], maxR = readings[0];
      for (const r of readings) {
        if (r.temp_c < minR.temp_c) minR = r;
        if (r.temp_c > maxR.temp_c) maxR = r;
      }
      return { min: minR, max: maxR };
    }

    function timeInBand(readings, lo, hi) {
      const MAX_GAP_MIN = 15;
      let total = 0;
      for (let i = 0; i < readings.length - 1; i++) {
        const cur = readings[i];
        const next = readings[i+1];
        if (!cur.ts || !next.ts) continue;
        const gapMin = (next.ts - cur.ts) / 60000;
        if (gapMin <= 0 || gapMin > MAX_GAP_MIN) continue;
        if (cur.temp_c >= lo && cur.temp_c < hi) {
          total += gapMin;
        }
      }
      return total;
    }

    function computeDailyTransitions(dayGroups) {
      const results = [];
      for (let i = 0; i < dayGroups.length; i++) {
        const day = dayGroups[i];
        const mm = minMax(day.readings);
        if (!mm || !mm.min.ts || !mm.max.ts) {
          results.push({ dateKey: day.dateKey, toMax: null, toMin: null });
          continue;
        }

        let toMax = null;
        const sameDayDiff = (mm.max.ts - mm.min.ts) / 60000;
        if (sameDayDiff > 0) toMax = sameDayDiff;

        let toMin = null;
        if (i + 1 < dayGroups.length) {
          const nextMm = minMax(dayGroups[i + 1].readings);
          if (nextMm && nextMm.min.ts && nextMm.min.ts > mm.max.ts) {
            toMin = (nextMm.min.ts - mm.max.ts) / 60000;
          }
        }

        results.push({ dateKey: day.dateKey, toMax, toMin });
      }
      return results;
    }

    function mostRecentCompleted(transitions, key) {
      for (let i = transitions.length - 1; i >= 0; i--) {
        if (transitions[i][key] !== null) return transitions[i][key];
      }
      return null;
    }

    function updateStats() {
      const readings = validReadings();
      if (readings.length === 0) { clearStats(); return; }
      showStats();

      const lastTs = readings[readings.length - 1].ts;
      const cutoff24h = new Date(lastTs.getTime() - 24*60*60*1000);
      const cutoff7d  = new Date(lastTs.getTime() - 7*24*60*60*1000);

      const last24h = readings.filter(r => r.ts >= cutoff24h);
      const last7d  = readings.filter(r => r.ts >= cutoff7d);

      const mm24 = minMax(last24h);
      if (mm24) {
        document.getElementById('stat24Max').textContent = fmtTemp(mm24.max.temp_c);
        document.getElementById('stat24MaxTime').textContent = `${mm24.max.day} ${mm24.max.time}`;
        document.getElementById('stat24Min').textContent = fmtTemp(mm24.min.temp_c);
        document.getElementById('stat24MinTime').textContent = `${mm24.min.day} ${mm24.min.time}`;
      } else {
        document.getElementById('stat24Max').textContent = '--';
        document.getElementById('stat24Min').textContent = '--';
        document.getElementById('stat24MaxTime').textContent = '';
        document.getElementById('stat24MinTime').textContent = '';
      }

      const mm7 = minMax(last7d);
      if (mm7) {
        document.getElementById('stat7Max').textContent = fmtTemp(mm7.max.temp_c);
        document.getElementById('stat7MaxTime').textContent = `${mm7.max.day} ${mm7.max.time}`;
        document.getElementById('stat7Min').textContent = fmtTemp(mm7.min.temp_c);
        document.getElementById('stat7MinTime').textContent = `${mm7.min.day} ${mm7.min.time}`;
      } else {
        document.getElementById('stat7Max').textContent = '--';
        document.getElementById('stat7Min').textContent = '--';
        document.getElementById('stat7MaxTime').textContent = '';
        document.getElementById('stat7MinTime').textContent = '';
      }

      const HOT_THRESHOLD_C = 25;
      const COLD_THRESHOLD_C = 15;
      const aboveMins = timeInBand(last7d, HOT_THRESHOLD_C, Infinity);
      const midMins   = timeInBand(last7d, COLD_THRESHOLD_C, HOT_THRESHOLD_C);
      const belowMins = timeInBand(last7d, -Infinity, COLD_THRESHOLD_C);
      document.getElementById('statAbove25').textContent = fmtHM(aboveMins);
      document.getElementById('statMid1525').textContent = fmtHM(midMins);
      document.getElementById('statBelow15').textContent = fmtHM(belowMins);

      const hotLabelC = HOT_THRESHOLD_C;
      const coldLabelC = COLD_THRESHOLD_C;
      const hotLabelF = Math.round(HOT_THRESHOLD_C * 9/5 + 32);
      const coldLabelF = Math.round(COLD_THRESHOLD_C * 9/5 + 32);
      if (currentUnit === 'C') {
        document.getElementById('labelAboveThreshold').innerHTML = `Above ${hotLabelC}&deg;C`;
        document.getElementById('labelMidThreshold').innerHTML = `${coldLabelC}&ndash;${hotLabelC}&deg;C`;
        document.getElementById('labelBelowThreshold').innerHTML = `Below ${coldLabelC}&deg;C`;
      } else {
        document.getElementById('labelAboveThreshold').innerHTML = `Above ${hotLabelF}&deg;F`;
        document.getElementById('labelMidThreshold').innerHTML = `${coldLabelF}&ndash;${hotLabelF}&deg;F`;
        document.getElementById('labelBelowThreshold').innerHTML = `Below ${coldLabelF}&deg;F`;
      }

      const dayGroups = groupByDay(last7d);
      if (dayGroups.length > 0) {
        const transitions = computeDailyTransitions(dayGroups);

        const latestMax = mostRecentCompleted(transitions, 'toMax');
        const latestMin = mostRecentCompleted(transitions, 'toMin');
        document.getElementById('statMaxToday').textContent = latestMax !== null ? fmtDuration(latestMax) : 'N/A';
        document.getElementById('statMinToday').textContent = latestMin !== null ? fmtDuration(latestMin) : 'N/A';

        const maxVals = transitions.filter(t => t.toMax !== null).map(t => t.toMax);
        const minVals = transitions.filter(t => t.toMin !== null).map(t => t.toMin);
        const avg = arr => arr.length ? arr.reduce((a,b) => a+b, 0) / arr.length : null;
        const avgMax = avg(maxVals);
        const avgMin = avg(minVals);
        document.getElementById('statMaxAvg7').textContent = avgMax !== null ? fmtDuration(avgMax) : 'N/A';
        document.getElementById('statMinAvg7').textContent = avgMin !== null ? fmtDuration(avgMin) : 'N/A';
      } else {
        ['statMaxToday','statMinToday','statMaxAvg7','statMinAvg7'].forEach(id => {
          document.getElementById(id).textContent = '--';
        });
      }

      const swingBody = document.getElementById('swingTableBody');
      swingBody.innerHTML = '';
      if (dayGroups.length === 0) {
        swingBody.innerHTML = '<tr><td colspan="4" class="no-data-msg">No data in the last 7 days.</td></tr>';
      } else {
        for (let i = dayGroups.length - 1; i >= 0; i--) {
          const g = dayGroups[i];
          const mm = minMax(g.readings);
          if (!mm) continue;
          const swing = mm.max.temp_c - mm.min.temp_c;
          const tr = document.createElement('tr');
          tr.innerHTML = `<td>${g.dateKey}</td>` +
                          `<td>${fmtTemp(mm.min.temp_c)}</td>` +
                          `<td>${fmtTemp(mm.max.temp_c)}</td>` +
                          `<td><strong>${fmtTempDelta(swing)}</strong></td>`;
          swingBody.appendChild(tr);
        }
      }

      const hourSums = new Array(24).fill(0);
      const hourCounts = new Array(24).fill(0);
      for (const r of readings) {
        const h = r.ts.getHours();
        hourSums[h] += r.temp_c;
        hourCounts[h]++;
      }
      const hourLabels = [];
      const hourAverages = [];
      for (let h = 0; h < 24; h++) {
        hourLabels.push(h.toString().padStart(2,'0') + ':00');
        hourAverages.push(hourCounts[h] > 0 ? (currentUnit === 'C' ? hourSums[h]/hourCounts[h] : (hourSums[h]/hourCounts[h])*9/5+32) : null);
      }
      diurnalChart.data.labels = hourLabels;
      diurnalChart.data.datasets[0].label = `Avg Temperature (°${currentUnit})`;
      diurnalChart.data.datasets[0].data = hourAverages;
      diurnalChart.options.scales.y.title.text = `Avg Temperature (°${currentUnit})`;
      diurnalChart.update();
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
  Serial.println("ESP32 BME280 Web Server: Charts & Stats Ready!");
}

void loop() {
  // Timer
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    logData();
  }
  delay(10);
}
