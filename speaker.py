import numpy as np
import serial
import numpy as np
import sounddevice
import time

from numpy import sin, linspace, pi, arange
from pylab import plot, show, title, xlabel, ylabel, subplot
from scipy import fft

import numpy as np
import matplotlib.pyplot as plt

def plot_frequency_spectrum(signal, sampling_rate):
    """
    Plot the frequency spectrum of a signal.

    Parameters:
    signal (array_like): The input signal.
    sampling_rate (float): The sampling rate of the signal.

    Returns:
    None
    """
    # Calculate the FFT
    fft_output = np.fft.fft(signal)
    
    # Calculate the frequencies corresponding to the FFT output
    frequencies = np.fft.fftfreq(len(signal), 1 / sampling_rate)
    
    # Take the absolute value of the FFT output to get the magnitude spectrum
    magnitude_spectrum = np.abs(fft_output)
    
    # Plot the frequency spectrum
    plt.figure(figsize=(10, 5))
    plt.plot(frequencies[:len(frequencies)//2], magnitude_spectrum[:len(frequencies)//2])
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.title('Frequency Spectrum')
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
  # Initialize serial connection
  ser = serial.Serial('COM8', 115200)  # Specify port and baud rate

  # Define the number of data points to sample
  sample_duration = 3
  fs = 4000
  num_samples = fs * sample_duration

  # Create an empty NumPy array to store the sampled data
  data_array = np.zeros(num_samples)

  # Counter to keep track of the number of samples
  sample_count = 0

  try:
      while sample_count < num_samples:
          # Read data from serial port
          data = ser.readline().strip()
          
          # Convert the data to a float (assuming incoming data is numerical)
          try:
              value = float(data.decode('utf-8'))
          except ValueError:
              print("Invalid data received:", data)
              continue
          
          # Store the sampled data in the NumPy array
          data_array[sample_count] = value
          
          # Increment sample count
          sample_count += 1
      
      print("Sampling complete.")
      
  finally:
      # Close the serial connection
      ser.close()

  # Print the sampled data array
  print("Sampled Data Array:", data_array)

  # Find the minimum and maximum values in the array
  min_val = np.min(data_array)
  max_val = np.max(data_array)

  # Normalize the array between -0.5 and 0.5
  normalized_array = ((data_array - min_val) / (max_val - min_val)) - 0.5
  sound  = (normalized_array*32768).astype(np.int16)  # scale to int16 for sound card

  print("Sound", sound)

  sounddevice.play(sound, fs)  # releases GIL
  time.sleep(sample_duration)  # NOTE: Since sound playback is async, allow sound playback to finish before Python exits

  plot_frequency_spectrum(sound, fs)



