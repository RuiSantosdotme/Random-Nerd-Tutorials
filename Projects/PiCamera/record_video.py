import picamera

camera = picamera.PiCamera()

camera.resolution = (640, 480)
camera.start_recording('test_video.h264')
camera.wait_recording(5)
camera.stop_recording()

print('Finished recording')
