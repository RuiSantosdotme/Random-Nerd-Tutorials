# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-picamera2-python/
 
from picamera2 import Picamera2, Preview
import time

picam2 = Picamera2()

camera_config = picam2.create_preview_configuration()
picam2.configure(camera_config)

picam2.start_preview(Preview.QTGL)
picam2.start()

time.sleep(2)

picam2.capture_file("test_photo.jpg")