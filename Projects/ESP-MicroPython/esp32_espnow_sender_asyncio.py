# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp-now-esp32/
 
import network
import aioespnow
import asyncio
import time

# Stats tracking
last_stats_time = time.time()
stats_interval = 10  # Print stats every 10 seconds

# Initialize Wi-Fi in station mode
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()

# Initialize AIOESPNow
e = aioespnow.AIOESPNow()
try:
    e.active(True)
except OSError as err:
    print("Failed to initialize AIOESPNow:", err)
    raise

# Receiver's MAC address (broadcast for testing)
#receiver_mac = b'\x30\x0e\xa4\xf6\x7d\x4c'
receiver_mac = b'\xff\xff\xff\xff\xff\xff'

# Add peer
try:
    e.add_peer(receiver_mac)
except OSError as err:
    print("Failed to add peer:", err)
    raise

def print_stats():
    stats = e.stats()
    print("\nESP-NOW Statistics:")
    print(f"  Packets Sent: {stats[0]}")
    print(f"  Packets Delivered: {stats[1]}")
    print(f"  Packets Dropped (TX): {stats[2]}")
    print(f"  Packets Received: {stats[3]}")
    print(f"  Packets Dropped (RX): {stats[4]}")

# Async function to send messages
async def send_messages(e, peer):
    message_count = 0
    while True:
        try:
            message = f"Hello! AIOESPNow message #{message_count}"
            if await e.asend(peer, message, sync=True):
                print(f"Sent message: {message}")
            else:
                print("Failed to send message")
            
            message_count += 1
            
            # Print stats every 10 seconds
            if time.time() - last_stats_time >= stats_interval:
                print_stats()
            
            await asyncio.sleep(1)  # Send every 1 second
        
        except OSError as err:
            print("Error:", err)
            await asyncio.sleep(5)

# Main async function
async def main(e, peer):
    await send_messages(e, peer)

# Run the async program
try:
    asyncio.run(main(e, receiver_mac))
except KeyboardInterrupt:
    print("Stopping sender...")
    e.active(False)
    sta.active(False)