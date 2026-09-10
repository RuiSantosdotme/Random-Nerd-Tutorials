/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-8x8-matrix-shape-editor-web-server/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <FastLED.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

#define DIN_PIN       2
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8
#define NUM_LEDS      (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS    200        // Brightness (0-255)
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB
#define ZIGZAG        false      // Set false if your panel is wired in straight rows instead of zigzag
#define FLIP_H        true       // Set true if the image displays mirrored left to right
#define FLIP_V        true       // Set true if the image displays upside down

#define NUM_SLOTS     10         // Number of stored shape slots
#define SHAPES_DIR    "/shapes"

CRGB leds[NUM_LEDS];
AsyncWebServer server(80);

CRGB    currentPixelColors[MATRIX_HEIGHT][MATRIX_WIDTH]; // Colors per pixel for the shape currently loaded into memory
uint8_t currentBrightness = 128;
bool    haveShapeLoaded   = false;
int     currentSlot       = -1;    // Which saved slot is on screen (-1 = preview/none)

// Slideshow state
bool     slideshowActive   = false;
uint32_t slideshowInterval = 2000;   // Delay in ms between shapes
uint32_t slideshowLastTick = 0;
int      slideshowIndex    = -1;     // Last slot shown (-1 = none)
int      slideshowOrder[NUM_SLOTS];  // Slots that currently hold a shape
int      slideshowCount    = 0;

// Convert (x,y) coordinates to the corresponding index in the matrix
uint16_t coordToIndex(uint8_t x, uint8_t y) {
  if (FLIP_H) x = (MATRIX_WIDTH - 1) - x;
  if (FLIP_V) y = (MATRIX_HEIGHT - 1) - y;
  if (ZIGZAG && (y & 0x01)) {
    x = (MATRIX_WIDTH - 1) - x;
  }
  return (y * MATRIX_WIDTH) + x;
}

// Set the color of a pixel at (x,y)
void setPixel(uint8_t x, uint8_t y, CRGB color) {
  if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;
  leds[coordToIndex(x, y)] = color;
}

// Renders currentPixelColors (scaled by currentBrightness) to the physical LEDs
void renderCurrentShape() {
  FastLED.clear();
  if (haveShapeLoaded) {
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        CRGB c = currentPixelColors[y][x];
        if (c == CRGB::Black) continue;
        c.nscale8_video(currentBrightness);
        setPixel(x, y, c);
      }
    }
  }
  FastLED.show();
}

// Clears the matrix and resets the current shape state
void clearMatrix() {
  haveShapeLoaded = false;
  currentSlot = -1;
  FastLED.clear();
  FastLED.show();
}

// Converts a JSON array of {r,g,b} pixel objects into a 2D CRGB LED matrix grid
bool jsonPixelsToGrid(JsonArray pixels, CRGB out[MATRIX_HEIGHT][MATRIX_WIDTH], bool clampUntrusted) {
  if (pixels.isNull() || pixels.size() != NUM_LEDS) return false;

  int i = 0;
  for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      JsonObject px = pixels[i].as<JsonObject>();
      if (px.isNull()) return false;
      int r = px["r"] | 0;
      int g = px["g"] | 0;
      int b = px["b"] | 0;
      if (clampUntrusted) {
        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);
      }
      out[y][x] = CRGB(r, g, b);
      i++;
    }
  }
  return true;
}

// Returns the filesystem path for a given slot number (0-9)
bool slotPath(int slot, char* outPath, size_t outLen) {
  if (slot < 0 || slot >= NUM_SLOTS) return false;
  snprintf(outPath, outLen, "%s/%d.json", SHAPES_DIR, slot);
  return true;
}

// Checks if a given slot has a saved shape (file exists)
bool slotExists(int slot) {
  char path[32];
  if (!slotPath(slot, path, sizeof(path))) return false;
  return LittleFS.exists(path);
}

// Loads a saved shape from a slot into currentPixelColors/currentBrightness
bool loadSlotToCurrent(int slot) {
  char path[32];
  if (!slotPath(slot, path, sizeof(path))) return false;
  if (!LittleFS.exists(path)) return false;

  File f = LittleFS.open(path, "r");
  if (!f) return false;

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) {
    Serial.printf("loadSlotToCurrent: JSON parse failed for slot %d: %s\n", slot, err.c_str());
    return false;
  }

  if (!jsonPixelsToGrid(doc["pixels"].as<JsonArray>(), currentPixelColors, false)) {
    Serial.printf("loadSlotToCurrent: slot %d has bad pixel array\n", slot);
    return false;
  }

  currentBrightness = doc["brightness"] | 128;
  haveShapeLoaded = true;
  currentSlot = slot;
  return true;
}

// Saves a shape to a slot, validating the input first
bool saveSlot(int slot, const char* name, JsonArray pixels, uint8_t brightness) {
  char path[32];
  if (!slotPath(slot, path, sizeof(path))) return false;
  if (pixels.size() != NUM_LEDS) return false;

  for (JsonVariant v : pixels) {
    JsonObject o = v.as<JsonObject>();
    if (o.isNull() || !o["r"].is<int>() || !o["g"].is<int>() || !o["b"].is<int>()) {
      return false;
    }
  }

  JsonDocument doc;
  doc["name"] = name;
  JsonArray px = doc["pixels"].to<JsonArray>();
  for (JsonVariant v : pixels) {
    JsonObject src = v.as<JsonObject>();
    JsonObject dst = px.add<JsonObject>();
    dst["r"] = constrain(src["r"].as<int>(), 0, 255);
    dst["g"] = constrain(src["g"].as<int>(), 0, 255);
    dst["b"] = constrain(src["b"].as<int>(), 0, 255);
  }
  doc["brightness"] = brightness;

  File f = LittleFS.open(path, "w");
  if (!f) return false;
  bool ok = serializeJson(doc, f) > 0;
  f.close();
  return ok;
}

// Deletes a saved shape from a slot
bool deleteSlot(int slot) {
  char path[32];
  if (!slotPath(slot, path, sizeof(path))) return false;
  if (!LittleFS.exists(path)) return true; // already gone
  return LittleFS.remove(path);
}

// Refreshes the slideshowOrder array with the current occupied slots
void refreshSlideshowOrder() {
  slideshowCount = 0;
  for (int s = 0; s < NUM_SLOTS; s++) {
    if (slotExists(s)) {
      slideshowOrder[slideshowCount++] = s;
    }
  }
}

// Builds a JSON array describing every slot (used to populate the gallery)
String buildSlotListJson() {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  for (int s = 0; s < NUM_SLOTS; s++) {
    JsonObject o = arr.add<JsonObject>();
    o["slot"] = s;
    char path[32];
    slotPath(s, path, sizeof(path));

    if (LittleFS.exists(path)) {
      File f = LittleFS.open(path, "r");
      JsonDocument shapeDoc;
      DeserializationError err = deserializeJson(shapeDoc, f);
      f.close();
      if (!err) {
        o["occupied"] = true;
        o["name"] = shapeDoc["name"] | "";
        o["brightness"] = shapeDoc["brightness"] | 0;
        o["pixels"] = shapeDoc["pixels"];
      } else {
        o["occupied"] = false;
      }
    } else {
      o["occupied"] = false;
    }
  }

  String out;
  serializeJson(doc, out);
  return out;
}

// Web page served for the UI to control the 8x8 matrix
const char INDEX_HTML[] PROGMEM = R"HTMLPAGE(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>8x8 Matrix Shape Editor</title>
<style>
  :root { --bg: #14161a; --panel: #1d2025; --border: #2c3038; --accent: #1b78e2; --text: #eceef1; --text-dim: #9aa0ab; }
  * { box-sizing: border-box; }
  body { margin: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; background: var(--bg); color: var(--text); padding: 20px; }
  h1 { font-size: 30px; margin: 0 0 20px; }
  p.sub { color: var(--text-dim); margin: 0 0 20px; font-size: 13px; }
  .layout { display: grid; grid-template-columns: minmax(260px, 340px) 1fr; gap: 20px; max-width: 900px; }
  @media (max-width: 700px) { .layout { grid-template-columns: 1fr; } }
  .panel { background: var(--panel); border: 1px solid var(--border); border-radius: 10px; padding: 16px; }
  .panel + .panel { margin-top: 16px; }
  .grid8 { display: grid; grid-template-columns: repeat(8, 1fr); gap: 4px; aspect-ratio: 1; width: 100%; max-width: 320px; margin: 0 auto; }
  .cell { background: #2a2d33; border-radius: 3px; cursor: pointer; aspect-ratio: 1; border: 1px solid #33373f; }
  .cell.on { border-color: white; }
  .row { display: flex; align-items: center; gap: 10px; margin: 10px 0; }
  .row label { width: 90px; font-size: 13px; color: var(--text-dim); flex-shrink: 0; }
  input[type=range] { flex: 1; }
  input[type=color] { width: 42px; height: 30px; border: none; background: none; padding: 0; cursor: pointer; }
  input[type=text] { flex: 1; background: #14161a; border: 1px solid var(--border); color: var(--text); padding: 7px 9px; border-radius: 6px; font-size: 13px; }
  select { flex: 1; background: #14161a; border: 1px solid var(--border); color: var(--text); padding: 7px 9px; border-radius: 6px; font-size: 13px; }
  button { background: var(--accent); color: #1a0d12; border: none; padding: 9px 14px; border-radius: 6px; font-weight: 600; cursor: pointer; font-size: 13px; }
  button.secondary { background: #2a2d33; color: var(--text); }
  button.danger { background: #3a2229; color: #ff8fa3; }
  .btn-row { display: flex; gap: 8px; margin-top: 12px; flex-wrap: wrap; }
  .gallery { display: grid; grid-template-columns: repeat(auto-fill, minmax(90px, 1fr)); gap: 8px; }
  .slot-card { background: #14161a; border: 1px solid var(--border); border-radius: 8px; padding: 8px; text-align: center; cursor: pointer; font-size: 11px; }
  .slot-card:hover { border-color: var(--accent); }
  .slot-card .mini { display: grid; grid-template-columns: repeat(8, 1fr); gap: 1px; width: 60px; height: 60px; margin: 0 auto 6px; background: #000; border-radius: 3px; overflow: hidden; }
  .slot-card .mini div { background: #222; }
  .slot-card .label { color: var(--text-dim); }
  .slot-card.empty { opacity: 0.4; }
  .status { font-size: 12px; color: var(--text-dim); min-height: 16px; margin-top: 8px; }
  .toggle-row { display: flex; align-items: center; justify-content: space-between; }
  .switch { position: relative; width: 42px; height: 24px; }
  .switch input { opacity: 0; width: 0; height: 0; }
  .slider-toggle { position: absolute; cursor: pointer; inset: 0; background: #33373f; border-radius: 24px; transition: .2s; }
  .slider-toggle:before { content: ""; position: absolute; height: 18px; width: 18px; left: 3px; bottom: 3px; background: white; border-radius: 50%; transition: .2s; }
  input:checked + .slider-toggle { background: var(--accent); }
  input:checked + .slider-toggle:before { transform: translateX(18px); }
</style>
</head>
<body>
<h1>8x8 Matrix Shape Editor</h1>
<div class="layout">
  <div>
    <div class="panel">
      <div class="grid8" id="grid"></div>
      <div class="row">
        <label>Color</label>
        <input type="color" id="colorPicker" value="#ff0000">
      </div>
      <div class="row">
        <label>Brightness</label>
        <input type="range" id="brightness" min="0" max="255" value="128">
        <span id="brightnessVal" style="width:34px; text-align:right; font-size:12px; color:var(--text-dim);">128</span>
      </div>
      <div class="row">
        <label>Shape Name</label>
        <input type="text" id="shapeName" placeholder="e.g. Heart" maxlength="20">
      </div>
      <div class="row">
        <label>Slot</label>
        <select id="slotSelect"></select>
      </div>
      <div class="btn-row">
        <button id="btnPreview" class="secondary">Preview on Matrix</button>
        <button id="btnSave">Save to Slot</button>
        <button id="btnClearGrid" class="secondary">Clear Grid</button>
        <button id="btnTurnOff" class="danger">Turn Off Matrix</button>
      </div>
      <div class="status" id="drawStatus"></div>
    </div>
  </div>
  <div>
    <div class="panel">
      <div class="toggle-row">
        <strong style="font-size:14px;">Saved Shapes</strong>
      </div>
      <div class="gallery" id="gallery" style="margin-top:12px;"></div>
    </div>
    <div class="panel">
      <div class="toggle-row">
        <strong style="font-size:14px;">Slideshow Animation</strong>
        <label class="switch">
          <input type="checkbox" id="slideshowToggle">
          <span class="slider-toggle"></span>
        </label>
      </div>
      <div class="row" style="margin-top:14px;">
        <label>Interval</label>
        <input type="range" id="slideshowInterval" min="100" max="10000" step="100" value="1000">
        <span id="slideshowIntervalVal" style="width:50px; text-align:right; font-size:12px; color:var(--text-dim);">2000ms</span>
      </div>
      <div class="status" id="slideshowStatus"></div>
    </div>
  </div>
</div>

<script>
const W = 8, H = 8;
let pixelColors = new Array(W * H).fill(null);
let slots = [];
let loadedFromSlot = null;

const gridEl = document.getElementById('grid');
function buildGrid() {
  gridEl.innerHTML = '';
  for (let i = 0; i < W * H; i++) {
    const cell = document.createElement('div');
    cell.className = 'cell';
    cell.dataset.index = i;
    cell.addEventListener('click', () => {
      if (pixelColors[i]) {
        pixelColors[i] = null;
      } else {
        pixelColors[i] = hexToRgb(document.getElementById('colorPicker').value);
      }
      paintCell(cell, pixelColors[i]);
    });
    gridEl.appendChild(cell);
  }
}

function paintCell(cellEl, color) {
  if (color) {
    cellEl.classList.add('on');
    cellEl.style.background = rgbToHex(color.r, color.g, color.b);
  } else {
    cellEl.classList.remove('on');
    cellEl.style.background = '';
  }
}
function renderGridFromPixels() {
  const cells = gridEl.children;
  for (let i = 0; i < pixelColors.length; i++) {
    paintCell(cells[i], pixelColors[i]);
  }
}
buildGrid();

document.getElementById('btnClearGrid').addEventListener('click', () => {
  pixelColors = new Array(W * H).fill(null);
  renderGridFromPixels();
  loadedFromSlot = null;
});

const brightnessInput = document.getElementById('brightness');
const brightnessVal = document.getElementById('brightnessVal');
brightnessInput.addEventListener('input', () => {
  brightnessVal.textContent = brightnessInput.value;
});

const slotSelect = document.getElementById('slotSelect');
function populateSlotSelect() {
  slotSelect.innerHTML = '';
  for (let s = 0; s < 10; s++) {
    const opt = document.createElement('option');
    opt.value = s;
    const meta = slots.find(x => x.slot === s);
    opt.textContent = 'Slot ' + s + (meta && meta.occupied ? ' ' + (meta.name || 'unnamed') : ' (empty)');
    slotSelect.appendChild(opt);
  }
}

function findFirstEmptySlot() {
  for (let s = 0; s < 10; s++) {
    const meta = slots.find(x => x.slot === s);
    if (!meta || !meta.occupied) return s;
  }
  return null;
}

function loadShapeIntoEditor(slotData) {
  pixelColors = slotData.pixels.map(p => (p.r === 0 && p.g === 0 && p.b === 0) ? null : { r: p.r, g: p.g, b: p.b });
  renderGridFromPixels();

  document.getElementById('shapeName').value = slotData.name || '';
  brightnessInput.value = slotData.brightness;
  brightnessVal.textContent = slotData.brightness;

  loadedFromSlot = slotData.slot;
  const emptySlot = findFirstEmptySlot();
  slotSelect.value = (emptySlot !== null) ? emptySlot : slotData.slot;

  setStatus('drawStatus', 'Loaded "' + (slotData.name || ('Slot ' + slotData.slot)) + '" into the editor.');
}

async function apiGet(path) {
  const res = await fetch(path);
  if (!res.ok) throw new Error('Request failed: ' + path);
  return res.json();
}
async function apiPost(path, body) {
  const res = await fetch(path, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body)
  });
  if (!res.ok) throw new Error('Request failed: ' + path);
  return res.json();
}

function hexToRgb(hex) {
  const v = parseInt(hex.slice(1), 16);
  return { r: (v >> 16) & 255, g: (v >> 8) & 255, b: v & 255 };
}
function rgbToHex(r, g, b) {
  return '#' + [r, g, b].map(x => x.toString(16).padStart(2, '0')).join('');
}

function pixelsForWire() {
  return pixelColors.map(c => c ? c : { r: 0, g: 0, b: 0 });
}

document.getElementById('btnPreview').addEventListener('click', async () => {
  const brightness = parseInt(brightnessInput.value, 10);
  setStatus('drawStatus', 'Sending preview...');
  try {
    await apiPost('/api/preview', { pixels: pixelsForWire(), brightness });
    document.getElementById('slideshowToggle').checked = false;
    setStatus('drawStatus', 'Preview sent.');
  } catch (e) {
    setStatus('drawStatus', 'Error sending preview.');
  }
});

document.getElementById('btnSave').addEventListener('click', async () => {
  const brightness = parseInt(brightnessInput.value, 10);
  const name = document.getElementById('shapeName').value || 'Shape';
  const slot = parseInt(slotSelect.value, 10);
  setStatus('drawStatus', 'Saving...');
  try {
    const result = await apiPost('/api/shape/save', { slot, name, pixels: pixelsForWire(), brightness });
    if (result.ok) {
      setStatus('drawStatus', 'Saved to slot ' + slot + '.');
      loadedFromSlot = slot;
      await apiPost('/api/preview', { pixels: pixelsForWire(), brightness });
      document.getElementById('slideshowToggle').checked = false;
      await loadGallery();
    } else {
      setStatus('drawStatus', 'Save failed: ' + (result.error || 'unknown error'));
    }
  } catch (e) {
    setStatus('drawStatus', 'Error saving shape.');
  }
});

document.getElementById('btnTurnOff').addEventListener('click', async () => {
  await apiPost('/api/clear', {});
  setStatus('slideshowStatus', 'Slideshow animation turned off.');
  setStatus('drawStatus', 'Matrix turned off.');
  document.getElementById('slideshowToggle').checked = false;
});

const galleryEl = document.getElementById('gallery');
async function loadGallery() {
  slots = await apiGet('/api/shape/list');
  populateSlotSelect();
  galleryEl.innerHTML = '';
  slots.forEach(s => {
    const card = document.createElement('div');
    card.className = 'slot-card' + (s.occupied ? '' : ' empty');

    const mini = document.createElement('div');
    mini.className = 'mini';
    for (let i = 0; i < 64; i++) {
      const px = document.createElement('div');
      if (s.occupied && s.pixels && s.pixels[i]) {
        const p = s.pixels[i];
        if (!(p.r === 0 && p.g === 0 && p.b === 0)) {
          px.style.background = rgbToHex(p.r, p.g, p.b);
        }
      }
      mini.appendChild(px);
    }

    const label = document.createElement('div');
    label.className = 'label';
    label.textContent = s.occupied ? (s.name || ('Slot ' + s.slot)) : ('Slot ' + s.slot + ' (empty)');

    card.appendChild(mini);
    card.appendChild(label);

    if (s.occupied) {
      card.style.position = 'relative';

      const delBtn = document.createElement('div');
      delBtn.textContent = '✕';
      delBtn.title = 'Delete this shape';
      delBtn.style.cssText = 'position:absolute; top:2px; right:4px; color:#ff8fa3; font-size:11px; cursor:pointer; padding:2px 4px;';
      delBtn.addEventListener('click', async (e) => {
        e.stopPropagation();
        await apiPost('/api/shape/delete', { slot: s.slot });
        setStatus('drawStatus', 'Deleted slot ' + s.slot + '.');
        await loadGallery();
      });
      card.appendChild(delBtn);

      card.title = 'Click to display on the matrix and load into the editor';
      card.addEventListener('click', async () => {
        await apiPost('/api/shape/display', { slot: s.slot });
        document.getElementById('slideshowToggle').checked = false;
        loadShapeIntoEditor(s);
      });
    }
    galleryEl.appendChild(card);
  });
}

const slideshowToggle = document.getElementById('slideshowToggle');
const slideshowIntervalInput = document.getElementById('slideshowInterval');
const slideshowIntervalVal = document.getElementById('slideshowIntervalVal');

slideshowIntervalInput.addEventListener('input', () => {
  slideshowIntervalVal.textContent = slideshowIntervalInput.value + 'ms';
});

async function pushSlideshowState() {
  const active = slideshowToggle.checked;
  const interval = parseInt(slideshowIntervalInput.value, 10);
  try {
    await apiPost('/api/slideshow', { active, interval });
    setStatus('slideshowStatus', active ? 'Slideshow running (' + interval + 'ms).' : 'Slideshow stopped.');
  } catch (e) {
    setStatus('slideshowStatus', 'Error updating slideshow.');
  }
}
slideshowToggle.addEventListener('change', pushSlideshowState);
slideshowIntervalInput.addEventListener('change', pushSlideshowState);

function setStatus(elId, msg) {
  document.getElementById(elId).textContent = msg;
}

loadGallery();
</script>
</body>
</html>
)HTMLPAGE";

// Applies a JSON payload containing "pixels" and "brightness" to the current shape in memory
bool applyPixelPayload(JsonVariant& doc) {
  CRGB tmp[MATRIX_HEIGHT][MATRIX_WIDTH];
  if (!jsonPixelsToGrid(doc["pixels"].as<JsonArray>(), tmp, true)) return false;

  ::memcpy(currentPixelColors, tmp, sizeof(tmp));

  currentBrightness = doc["brightness"] | 128;
  haveShapeLoaded = true;
  currentSlot = -1;
  return true;
}

// Sends JSON response: {"ok":true} 
void sendOk(AsyncWebServerRequest* request) {
  request->send(200, "application/json", "{\"ok\":true}");
}

// Sends JSON response: {"ok":false,"error":"..."} 
void sendError(AsyncWebServerRequest* request, const char* msg) {
  JsonDocument resp;
  resp["ok"] = false;
  resp["error"] = msg;
  String out;
  serializeJson(resp, out);
  request->send(200, "application/json", out);
}

// Parses a JSON body containing "slot" and validates it is in the range 0-9
bool parseSlot(JsonObject doc, int& outSlot) {
  int slot = doc["slot"] | -1;
  if (slot < 0 || slot >= NUM_SLOTS) return false;
  outSlot = slot;
  return true;
}

// Sets up all the web server routes for the UI and API
void setupWebServerRoutes() {
  // Serve the UI web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", INDEX_HTML);
  });

  // GET /api/shape/list -> JSON array describing all 10 shape slots
  server.on("/api/shape/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", buildSlotListJson());
  });

  // POST /api/shape/save { slot, name, pixels[64], brightness } -> saves the shape to that slot
  AsyncCallbackJsonWebHandler* saveHandler = new AsyncCallbackJsonWebHandler(
    "/api/shape/save",
    [](AsyncWebServerRequest *request, JsonVariant &json) {
      JsonObject doc = json.as<JsonObject>();

      int slot;
      const char* name = doc["name"] | "Shape";
      JsonArray pixels = doc["pixels"].as<JsonArray>();
      uint8_t brightness = doc["brightness"] | 128;

      if (!parseSlot(doc, slot)) {
        sendError(request, "invalid slot");
      } else if (pixels.isNull() || pixels.size() != NUM_LEDS) {
        sendError(request, "invalid pixel data");
      } else if (!saveSlot(slot, name, pixels, brightness)) {
        sendError(request, "write failed");
      } else {
        refreshSlideshowOrder();
        sendOk(request);
      }
    }
  );
  server.addHandler(saveHandler);

  // POST /api/shape/display { slot } -> displays the shape from that slot
  AsyncCallbackJsonWebHandler* displayHandler = new AsyncCallbackJsonWebHandler(
    "/api/shape/display",
    [](AsyncWebServerRequest *request, JsonVariant &json) {
      JsonObject doc = json.as<JsonObject>();
      int slot;

      slideshowActive = false;

      if (!parseSlot(doc, slot) || !loadSlotToCurrent(slot)) {
        sendError(request, "slot empty or invalid");
        return;
      }
      renderCurrentShape();
      sendOk(request);
    }
  );
  server.addHandler(displayHandler);

  // POST /api/shape/delete { slot } -> deletes the shape from that slot
  AsyncCallbackJsonWebHandler* deleteHandler = new AsyncCallbackJsonWebHandler(
    "/api/shape/delete",
    [](AsyncWebServerRequest *request, JsonVariant &json) {
      JsonObject doc = json.as<JsonObject>();
      int slot;

      if (!parseSlot(doc, slot)) {
        sendError(request, "invalid slot");
        return;
      }

      bool ok = deleteSlot(slot);
      refreshSlideshowOrder();

      if (slot == currentSlot) {
        clearMatrix();
      }

      if (ok) sendOk(request);
      else sendError(request, "delete failed");
    }
  );
  server.addHandler(deleteHandler);

  // POST /api/preview { pixels[64] (each {r,g,b}), brightness } -> preview the shape
  AsyncCallbackJsonWebHandler* previewHandler = new AsyncCallbackJsonWebHandler(
    "/api/preview",
    [](AsyncWebServerRequest *request, JsonVariant &json) {
      slideshowActive = false;
      if (applyPixelPayload(json)) {
        renderCurrentShape();
        sendOk(request);
      } else {
        sendError(request, "invalid pixel data");
      }
    }
  );
  server.addHandler(previewHandler);

  // POST /api/clear {} -> turns the matrix off
  server.on("/api/clear", HTTP_POST, [](AsyncWebServerRequest *request) {
    slideshowActive = false;
    clearMatrix();
    sendOk(request);
  });

  // POST /api/slideshow { active: bool, interval: ms } -> starts/stops the slideshow animation and sets the interval
  AsyncCallbackJsonWebHandler* slideshowHandler = new AsyncCallbackJsonWebHandler(
    "/api/slideshow",
    [](AsyncWebServerRequest *request, JsonVariant &json) {
      JsonObject doc = json.as<JsonObject>();
      bool active = doc["active"] | false;
      uint32_t interval = doc["interval"] | 2000;
      if (interval < 250) interval = 250;

      refreshSlideshowOrder();

      slideshowInterval = interval;
      slideshowActive = active && (slideshowCount > 0);
      slideshowLastTick = millis();

      JsonDocument resp;
      resp["ok"] = true;
      resp["active"] = slideshowActive;
      resp["count"] = slideshowCount;
      String out;
      serializeJson(resp, out);
      request->send(200, "application/json", out);
    }
  );
  server.addHandler(slideshowHandler);

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });
}

// Updates the slideshow animation if it's active and enough time has passed since the last frame
void updateSlideshow() {
  if (!slideshowActive || slideshowCount == 0) return;

  uint32_t now = millis();
  if (now - slideshowLastTick < slideshowInterval) return;
  slideshowLastTick = now;

  slideshowIndex = (slideshowIndex + 1) % slideshowCount;
  int slot = slideshowOrder[slideshowIndex];

  if (loadSlotToCurrent(slot)) {
    renderCurrentShape();
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize LittleFS and create the shapes directory if it doesn't exist
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed even after format attempt.");
  }
  if (!LittleFS.exists(SHAPES_DIR)) {
    LittleFS.mkdir(SHAPES_DIR);
  }
  refreshSlideshowOrder();

  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, DIN_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

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

  // Set up routes requests and start the web server
  setupWebServerRoutes();
  server.begin();
  Serial.println("ESP32 Web Server started!");
}

void loop() {
  // Handle the slideshow animation when it's active
  updateSlideshow();
}
