# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-web-server-filesystem-micropython/

# Import necessary modules
import network
import socket
import time
from picozero import pico_temp_sensor #must install the picozero package
from machine import Pin
import json

ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Constant variables to save the files path
HTML_FILE_PATH = "index.html"
CSS_FILE_PATH = "style.css"
JS_FILE_PATH ="script.js"

# Create an LED object on pin 'LED' (this is the onboard LED)
led = Pin('LED', Pin.OUT)

# Initialize LED state
state = 'OFF'

# Function to read content from the file
def read_file(filepath):
    with open(filepath, "r") as file:
        return file.read()

# Get sensor readings
# Pico internal temperature sensor
def get_temperature():
    temperature_c = pico_temp_sensor.temp
    temperature_f = temperature_c * (9/5) + 32
    
    temperature_c = round(temperature_c)
    temperature_f = round(temperature_f)
    return temperature_c, temperature_f

# HTML template for the webpage
def webpage(state):
    html_content = read_file(HTML_FILE_PATH)
    temperature_c, temperature_f = get_temperature() 
    html = html_content.format(state=state, temperature_c=temperature_c, temperature_f=temperature_f)
    return html

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

                try:
                    path = request.split()[1]
                    print('Requested path:', path)
                except IndexError:
                    pass
                
                # Process the request and update variables19
                if path == b'/lighton':
                    print('LED on')
                    led.value(1)
                    state = 'ON'
                    response = json.dumps({"state": state})
                    content_type = 'application/json'
                    
                elif path == b'/lightoff':
                    print('LED off')
                    led.value(0)
                    state = 'OFF'
                    response = json.dumps({"state": state})
                    content_type = 'application/json'
                    
                elif path == b'/temperature':
                    print('New temperature value requested')
                    temperature_c, temperature_f = get_temperature()
                    data = {
                        "temperature_c": temperature_c,
                        "temperature_f": temperature_f
                    }
                    response = json.dumps(data)
                    content_type = 'application/json'
                    
                # process the request and send the files  
                elif path == b'/style.css':
                    response = read_file(CSS_FILE_PATH)
                    content_type = 'text/css'
                
                elif path == b'/script.js':
                    response = read_file(JS_FILE_PATH)
                    content_type = 'text/javascript'
                    
                else: # / root path
                    response = webpage(state)
                    content_type = 'text/html'

                # Send the HTTP response and close the connection
                conn.send(f'HTTP/1.0 200 OK\r\nContent-type: {content_type}\r\n\r\n')
                conn.send(response)
                conn.close()

            except OSError as e:
                conn.close()
                print('Connection closed')
                
    except KeyboardInterrupt:
            print('Server stopped by user.')
