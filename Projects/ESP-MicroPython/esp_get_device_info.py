# WhatsInIt.py - MicroPython Hardware & Environment Diagnostic Tool
# For ESP32 / ESP8266 boards
# Last updated: April 26, 2025 by Charles E. Hamilton
# This program is released into the public domain free of license and without warranties of any kind
# https://RandomNerdTutorials.com/micropython-esp32-esp8266-device-info/

import os
import sys
import gc
import machine
import network
import ubinascii

try:
    import esp
except ImportError:
    esp = None

try:
    import esp32
except ImportError:
    esp32 = None

print("\n" + "=" * 40)
print("        WHATS IN IT REPORT")
print("=" * 40)

# --- Chip & CPU Info ---
print("\n[CHIP INFO]")
print(f"Platform:           {sys.platform}")
print(f"MicroPython ver:    {os.uname().release} ({os.uname().version})")
print(f"Machine ID:         {os.uname().machine}")
print(f"CPU Frequency:      {machine.freq()} Hz")

# --- MAC Address ---
print("\n[NETWORK]")
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
mac = ubinascii.hexlify(wlan.config('mac'), ':').decode()
print(f"MAC Address:        {mac}")

# --- Memory Info ---
print("\n[MEMORY]")
print(f"Heap Allocated:     {gc.mem_alloc()} bytes")
print(f"Heap Free:          {gc.mem_free()} bytes")

# Flash size (ESP32 only)
if esp:
    try:
        flash_size = esp.flash_size()
        print(f"Flash Size:         {flash_size // (1024*1024)} MB")
    except:
        print("Flash Size:         Not available")

# PSRAM status (ESP32 only)
if esp32:
    try:
        psram_status = "Available" if esp32.ULP() is not None else "Not available"
    except:
        psram_status = "Not detected"
    print(f"PSRAM:              {psram_status}")

# --- Filesystem Info ---
print("\n[FILESYSTEM]")
try:
    fs_stats = os.statvfs("/")
    block_size = fs_stats[0]
    total_blocks = fs_stats[2]
    free_blocks = fs_stats[3]
    total = (block_size * total_blocks) // 1024
    free = (block_size * free_blocks) // 1024
    print(f"Total Size:         {total} KB")
    print(f"Free Space:         {free} KB")
except:
    print("Filesystem info:    Not available")

# --- Directory Contents ---
print("\n[FILES IN ROOT DIR]")
try:
    files = os.listdir()
    for f in files:
        print(f" - {f}")
except:
    print("Cannot list files.")

# --- Closing ---
print("\n[END OF REPORT]")
print("=" * 40)
