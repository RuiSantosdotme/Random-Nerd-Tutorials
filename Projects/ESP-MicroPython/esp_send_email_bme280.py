# Complete project details: https://RandomNerdTutorials.com/micropython-sensor-readings-email-esp32-esp826/
import umail # Micropython lib to send emails: https://github.com/shawwwn/uMail
import network
import BME280
from machine import Pin, SoftI2C

# Your network credentials
ssid = 'REPLACE_WITH_YOUR_SSID'
password = 'REPLACE_WITH_YOUR_PASSWORD'

# Email details
sender_email = 'REPLACE_WITH_THE_SENDER_EMAIL'
sender_name = 'ESP32' #sender name
sender_app_password = 'REPLACE_WITH_THE_SENDER_EMAIL_APP_PASSWORD'
recipient_email ='REPLACE_WITH_THE_RECIPIENT_EMAIL'
email_subject ='BME280 Sensor Readings'

# BME280 pin assignment - ESP32
i2c = SoftI2C(scl=Pin(22), sda=Pin(21), freq=10000)
# BME280 pin assignment - ESP8266
#i2c = I2C(scl=Pin(5), sda=Pin(4), freq=10000)
bme = BME280.BME280(i2c=i2c)

def read_bme_sensor():
  try:
    temp = str(bme.temperature[:-1]) + " ºC"
    #uncomment for temperature in Fahrenheit
    #temp = str((bme.read_temperature()/100) * (9/5) + 32) + " 潞F"
    hum = str(bme.humidity[:-1]) + " %"
    pres = str(bme.pressure[:-3]) + " hPa"

    return temp, hum, pres
    #else:
    #  return('Invalid sensor readings.')
  except OSError as e:
    return('Failed to read sensor.')
  
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

# Get sensor readings
temp, hum, pres = read_bme_sensor()
print(temp)
print(hum)
print(pres)

# Send the email
smtp = umail.SMTP('smtp.gmail.com', 465, ssl=True) # Gmail's SSL port
smtp.login(sender_email, sender_app_password)
smtp.to(recipient_email)
smtp.write("From:" + sender_name + "<"+ sender_email+">\n")
smtp.write("Subject:" + email_subject + "\n")
smtp.write("Temperature " + temp + "\n")
smtp.write("Humidity " + hum + "\n")
smtp.write("Pressure " + pres + "\n")
smtp.send()
smtp.quit()
