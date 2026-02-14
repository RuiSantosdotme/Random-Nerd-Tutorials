# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp-now-esp32/

import network
import espnow
import time

# Stats tracking
last_stats_time = time.time()
stats_interval = 10  # Print stats every 10 seconds

# Initialize Wi-Fi in station mode
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.config(channel=1)  # Set channel explicitly if packets are not received
sta.disconnect()

# Initialize ESP-NOW
e = espnow.ESPNow()
try:
    e.active(True)
except OSError as err:
    print("Failed to initialize ESP-NOW:", err)
    raise

# Sender's MAC address
sender_mac = b'\x30\xae\xa4\x07\x0d\x64'  # Sender MAC

# Add peer (sender) for unicast reliability
# You don't need to add peer for broadcast
#try:
#    e.add_peer(sender_mac)
#except OSError as err:
#    print("Failed to add peer:", err)
#    raise

def print_stats():
    stats = e.stats()
    print("\nESP-NOW Statistics:")
    print(f"  Packets Sent: {stats[0]}")
    print(f"  Packets Delivered: {stats[1]}")
    print(f"  Packets Dropped (TX): {stats[2]}")
    print(f"  Packets Received: {stats[3]}")
    print(f"  Packets Dropped (RX): {stats[4]}")

print("Listening for ESP-NOW messages...")
while True:
    try:
        # Receive message (host MAC, message, timeout of 10 seconds)
        host, msg = e.recv(10000)
        if msg:
            print(f"Received from {host.hex()}: {msg.decode()}")
        # Print stats every 10 seconds
        if time.time() - last_stats_time >= stats_interval:
            print_stats()
            last_stats_time = time.time()
        
    except OSError as err:
        print("Error:", err)
        time.sleep(5)
        
    except KeyboardInterrupt:
        print("Stopping receiver...")
        e.active(False)
        sta.active(False)

        break
