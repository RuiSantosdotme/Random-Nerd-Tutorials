# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/micropython-esp-now-esp32/

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

# Sender's MAC address
sender_mac = b'\x30\xae\xa4\x07\x0d\x64'  # Sender MAC for unicast

# Add peer (sender) for unicast reliability
# You don't need to add peer for broadcast
#try:
#    e.add_peer(sender_mac)
#except OSError as err:
#    print("Failed to add peer:", err)
#    raise

# Async function to receive messages
async def receive_messages(e):
    while True:
        try:
            async for mac, msg in e:
                print(f"Received from {mac.hex()}: {msg.decode()}")
        except OSError as err:
            print("Error:", err)
            await asyncio.sleep(5)

# Async function to print stats periodically
async def print_stats(e):
    global last_stats_time
    last_stats_time = time.time()
    stats_interval = 10  # Print stats every 10 seconds
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
async def main(e):
    # Run receive and stats tasks concurrently
    await asyncio.gather(receive_messages(e), print_stats(e))

# Run the async program
try:
    asyncio.run(main(e))
except KeyboardInterrupt:
    print("Stopping receiver...")
    e.active(False)
    sta.active(False)