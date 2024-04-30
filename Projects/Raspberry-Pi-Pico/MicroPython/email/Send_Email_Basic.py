# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-w-send-email-micropython/
# Micropython lib to send emails: https://github.com/shawwwn/uMail
import umail
import network
import time

# Your network credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Email details
sender_email = 'REPLACE_WITH_THE_SENDER_EMAIL'
sender_name = 'Raspberry Pi Pico'
sender_app_password = 'REPLACE_WITH_THE_SENDER_EMAIL_APP_PASSWORD'
recipient_email ='REPLACE_WITH_THE_RECIPIENT_EMAIL'
email_subject ='Hello from RPi Pico W'

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
    
# Connect to your network
init_wifi(ssid, password)

# Send the email
smtp = umail.SMTP('smtp.gmail.com', 465, ssl=True) # Gmail's SSL port

try:
    smtp.login(sender_email, sender_app_password)
    smtp.to(recipient_email)
    smtp.write("From:" + sender_name + "<"+ sender_email+">\n")
    smtp.write("Subject:" + email_subject + "\n")
    smtp.write("Hello from the Raspberry Pi Pico. Testing.")
    smtp.send()
    print("Email Sent Successfully")
    
except Exception as e:
    print("Failed to send email:", e)
finally:
    smtp.quit()