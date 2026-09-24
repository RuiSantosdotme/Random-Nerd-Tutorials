# Complete project details at https://RandomNerdTutorials.com/esp32-inmp441-i2s-microphone-arduino/
import serial
import matplotlib.pyplot as plt

PORT = "COM6"
BAUD_RATE = 115200
NUM_SAMPLES = 512

ser = serial.Serial(PORT, BAUD_RATE, timeout=1)

print("Reading samples from ESP32...")
print("Close the plot window to stop.")

plt.ion()
fig, ax = plt.subplots()

while True:
    samples = []

    while len(samples) < NUM_SAMPLES:
        line = ser.readline().decode("utf-8", errors="ignore").strip()

        if line:
            try:
                samples.append(int(line))
            except ValueError:
                pass

    ax.clear()
    ax.plot(samples)

    ax.set_title("INMP441 Audio Waveform")
    ax.set_xlabel("Sample")
    ax.set_ylabel("Amplitude")
    ax.grid(True)

    plt.pause(0.001)

ser.close()
