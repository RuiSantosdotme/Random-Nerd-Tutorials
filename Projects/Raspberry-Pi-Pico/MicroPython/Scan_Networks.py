# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-w-wi-fi-micropython/

import network

# Init Wi-Fi interface
wlan = network.WLAN(network.STA_IF)
wlan.active(True)

# Scan for Wi-Fi networks
networks = wlan.scan()

# Print Wi-Fi networks
print("Available WiFi Networks:")
for network_info in networks:
    print(network_info)