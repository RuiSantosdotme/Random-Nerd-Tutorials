# Complete project details: https://RandomNerdTutorials.com/micropython-send-emails-esp32-esp826/
# Micropython lib to send emails: https://github.com/shawwwn/uMail
import umail
import network

# Your network credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Email details
sender_email = 'REPLACE_WITH_THE_SENDER_EMAIL'
sender_name = 'ESP32' #sender name
sender_app_password = 'REPLACE_WITH_THE_SENDER_EMAIL_APP_PASSWORD'
recipient_email ='REPLACE_WITH_THE_RECIPIENT_EMAIL'
email_subject ='Test Email'

def connect_wifi(ssid, password):
  #Connect to your network
  station = network.WLAN(network.STA_IF)
  station.active(True)
  station.connect(ssid, password)
  while station.isconnected() == False:
    pass
  print('Connection successful')
  print(station.ifconfig())
    
# Connect to your network
connect_wifi(ssid, password)

# Send the email
smtp = umail.SMTP('smtp.gmail.com', 465, ssl=True) # Gmail's SSL port
smtp.login(sender_email, sender_app_password)
smtp.to(recipient_email)
smtp.write("From:" + sender_name + "<"+ sender_email+">\n")
smtp.write("Subject:" + email_subject + "\n")
smtp.write("Hello from ESP32")
smtp.send()
smtp.quit()
