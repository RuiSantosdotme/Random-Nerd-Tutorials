# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-picamera2-python/

from picamera2 import Picamera2

picam2 = Picamera2()

picam2.start_and_record_video("test_video.mp4", duration=5)