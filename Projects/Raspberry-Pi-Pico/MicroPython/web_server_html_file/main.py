# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-web-server-filesystem-micropython/

# Import necessary modules
import network
import socket
import time

# Wi-Fi credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Variable to save the HTML file path
HTML_FILE_PATH = "index.html"

# Function to read and return HTML content from the file
def read_file(filepath):
    with open(filepath, "r") as file:
        return file.read()

# Init Wi-Fi Interface
def init_wifi(ssid, password):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    # Connect to your network
    wlan.connect(ssid, password)
    # Wait for Wi-Fi connection
    connection_timeout = 10
    while connection_timeout > 0:
        print(wlan.status())
        if wlan.status() >= 3:
            break
        connection_timeout -= 1
        print('Waiting for Wi-Fi connection...')
        time.sleep(1)
    # Check if connection is successful
    if wlan.status() != 3:
        print('Failed to connect to Wi-Fi')
        return False
    else:
        print('Connection successful!')
        network_info = wlan.ifconfig()
        print('IP address:', network_info[0])
        return True

if not init_wifi(ssid, password):
    print("Exiting program.")
else:
    try:
        # Set up socket and start listening
        addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
        s = socket.socket()
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(addr)
        s.listen()
        print('Listening on', addr)

        # Main loop to listen for connections
        while True:
            try:
                conn, addr = s.accept()
                print('Got a connection from', addr)
                
                # Receive and parse the request
                request = conn.recv(1024)
                request_str = request.decode('utf-8')
                print('Request content:')

                # Generate HTML response
                response = read_file(HTML_FILE_PATH)  

                # Send the HTTP response and close the connection
                conn.send('HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n')
                conn.send(response)
                conn.close()

            except OSError as e:
                conn.close()
                print('Connection closed')
                
    except KeyboardInterrupt:
            print('Server stopped by user.')