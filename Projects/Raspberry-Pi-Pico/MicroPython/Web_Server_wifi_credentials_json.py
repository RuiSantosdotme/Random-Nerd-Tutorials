# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details: https://RandomNerdTutorials.com/raspberry-pi-pico-w-save-network-credentials/

import network
import socket
import time
import json

# HTML template for the webpage
webpage = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>Pico Web Server</title>
            <meta name="viewport" content="width=device-width, initial-scale=1">
        </head>
        <body>
            <h1>Raspberry Pi Pico Web Server</h1>
            <p>Hello, World!</p>
        </body>
        </html>
        """

def init_wifi_from_file(file_path='wifi_credentials.json'):
    try:
        with open(file_path, 'r') as file:
            credentials = json.load(file)
    except OSError:
        print(f"Error: Unable to read {file_path}. Make sure the file exists and is properly formatted.")
        return False

    for cred in credentials:
        ssid = cred.get('ssid')
        password = cred.get('password')
        if ssid and password:
            if init_wifi(ssid, password):
                return True

    print("Unable to connect to any Wi-Fi network.")
    return False

# Init Wi-Fi Interface
def init_wifi(ssid, password):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.disconnect()

    print(f"Trying to connect to Wi-Fi network with SSID: {ssid}")

    wlan.connect(ssid, password)

    # Wait for Wi-Fi connection
    connection_timeout = 10
    while connection_timeout > 0:
        if wlan.isconnected():
            print('Connection successful!')
            network_info = wlan.ifconfig()
            print('IP address:', network_info[0])
            return True

        connection_timeout -= 1
        print('Waiting for Wi-Fi connection...')
        time.sleep(1)

    print(f"Failed to connect to Wi-Fi network with SSID: {ssid}")
    return False

# Set up Wi-Fi
if not init_wifi_from_file():
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
                
                # Split headers and body
                headers, body = request_str.split('\r\n\r\n', 1)
                print('Headers:\n', headers)
                print('Body:\n', body)

                # Generate HTML response
                response = webpage

                # Send the HTTP response and close the connection
                conn.send('HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n')
                conn.send(response)
                conn.close()

            except OSError as e:
                conn.close()
                print('Connection closed')

    except KeyboardInterrupt:
        print('Server stopped by user.')
