# Rui Santos & Sara Santos - Random Nerd Tutorials
# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-files-directories-micropython/

import os

# Create a new file called testFile.txt
file = open('testFile.txt', 'w')

# List all files in the filesystem
print(os.listdir())

# Write content to the file
file.write("Writing my first line \n")
file.close()

# Rename the file
os.rename('testFile.txt','newNameFile.txt')

# List all files in the filesystem
print(os.listdir())

# Open the file in appending mode
file = open('newNameFile.txt', 'a')
# Append data to the file
file.write('This line was appended')
file.close()

# Open file in reading mode
file = open('newNameFile.txt', 'r')

# Read the content of the file
content = file.read()

# Print the content of the file
print("File content:")
print(content)
file.close()
