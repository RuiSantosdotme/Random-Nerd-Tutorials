# OTA updater for ESP32 running Micropython by David Flory
# Tutorial: https://randomnerdtutorials.com/esp32-esp8266-micropython-ota-updates/

try:
  import usocket as socket
except:
  import socket

try:
  import urequests as requests
except:
  import requests
import os 
import sys
import esp
esp.osdebug(None)
import gc
gc.collect()
from utime import sleep
from utime import sleep_ms
import network

OTA = 0