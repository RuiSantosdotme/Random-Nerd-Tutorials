# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-send-email-python-smtp-server/

import smtplib
from email.message import EmailMessage

#Set the sender email and password and recipient emaiç
from_email_addr ="REPLACE_WITH_THE_SENDER_EMAIL"
from_email_pass ="REPLACE_WITH_THE_SENDER_EMAIL_APP_PASSWORD"
to_email_addr ="REPLACE_WITH_THE_RECIPIENT_EMAIL"

# Create a message object
msg = EmailMessage()

# Set the email body
body ="Hello from Raspberry Pi"
msg.set_content(body)

# Set sender and recipient
msg['From'] = from_email_addr
msg['To'] = to_email_addr

# Set your email subject
msg['Subject'] = 'TEST EMAIL'

# Connecting to server and sending email
# Edit the following line with your provider's SMTP server details
server = smtplib.SMTP('smtp.gmail.com', 587)

# Comment out the next line if your email provider doesn't use TLS
server.starttls()
# Login to the SMTP server
server.login(from_email_addr, from_email_pass)

# Send the message
server.send_message(msg)

print('Email sent')

#Disconnect from the Server
server.quit()
