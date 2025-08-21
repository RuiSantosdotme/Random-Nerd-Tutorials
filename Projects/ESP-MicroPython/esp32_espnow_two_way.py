# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp32-esp-now-two-way/

import network
import aioespnow
import asyncio
import time

# Initialize Wi-Fi in station mode
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.config(channel=1)  # Set channel explicitly if packets are not received
sta.disconnect()

# Initialize AIOESPNow
e = aioespnow.AIOESPNow()
try:
    e.active(True)
except OSError as err:
    print("Failed to initialize AIOESPNow:", err)
    raise

# Peer MAC address (replace with the actual MAC of the other board)
peer_mac = b'\x30\xae\xa4\xf6\x7d\x4c'  # Example peer MAC for unicast

# Add peer for unicast reliability
try:
    e.add_peer(peer_mac)
except OSError as err:
    print("Failed to add peer:", err)
    raise

# Stats tracking
last_stats_time = time.time()
stats_interval = 10  # Print stats every 10 seconds

# Async function to send messages
async def send_messages(e, peer):
    message_count = 0
    while True:
        try:
            message = f"Hello from ESP32 #{message_count}"
            if await e.asend(peer, message, sync=True):
                print(f"Sent message: {message}")
            else:
                print("Failed to send message")
            message_count += 1
            await asyncio.sleep(1)  # Send every 1 second
        except OSError as err:
            print("Send error:", err)
            await asyncio.sleep(5)

# Async function to receive messages
async def receive_messages(e):
    while True:
        try:
            async for mac, msg in e:
                print(f"Received from {mac.hex()}: {msg.decode()}")
        except OSError as err:
            print("Receive error:", err)
            await asyncio.sleep(5)

# Async function to print stats periodically
async def print_stats(e):
    global last_stats_time
    while True:
        if time.time() - last_stats_time >= stats_interval:
            stats = e.stats()
            print("\nESP-NOW Statistics:")
            print(f"  Packets Sent: {stats[0]}")
            print(f"  Packets Delivered: {stats[1]}")
            print(f"  Packets Dropped (TX): {stats[2]}")
            print(f"  Packets Received: {stats[3]}")
            print(f"  Packets Dropped (RX): {stats[4]}")
            last_stats_time = time.time()
        await asyncio.sleep(1)  # Check every second

# Main async function
async def main(e, peer):
    # Run send, receive, and stats tasks concurrently
    await asyncio.gather(send_messages(e, peer), receive_messages(e), print_stats(e))

# Run the async program
try:
    asyncio.run(main(e, peer_mac))
except KeyboardInterrupt:
    print("Stopping transceiver...")
    e.active(False)
    sta.active(False)
