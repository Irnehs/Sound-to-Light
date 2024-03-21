/* This program uses a microphone, LED, and photoresistor to sample environment audio,
 * peform digital signal processing on saved values, then output the processed signal 
 * to light via LED and fiber optics cable. That output is then picked up by a photoresistor, 
 * which will later be configured to drive a speaker to play the processed audio.
 *
 * Author: Henri Schulz
 * Date: 3/19/2024
 */

// Sampling variables
#define SAMPLE_FREQ 4000 // max frequency is ~5k Hz depending on code changes
#define SAMPLE_SIZE 64
int samples[SAMPLE_SIZE]; // buffer of SAMPLE_SIZE to store samples
int processed[SAMPLE_SIZE]; // buffer of processed samples after DSP
int period = 1e6 / SAMPLE_FREQ; // determines period of each sample
int mic_sample; // used to temporarily hold sample of the mic
int sample_index = 0; // tracks index of sample in buffer
int startTime; // tracks start time of a sampling window to compute sampling frequency
bool processed_data = false;

// Pin mappings
const int LED_PIN = 25;
const int MIC_PIN = 27;
const int PHOTO_PIN = 26;

// Sets up pins and Serial monitor once
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(MIC_PIN, INPUT);
  pinMode(PHOTO_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  if (sampleMic()) { // If mic sample has been updated... (Note: keeps all reads and writes on same frequency)
    readFromLight(); // Reads photoresistor value and displays on Serial monitor
    if (sample_index == SAMPLE_SIZE) { // If sample buffer is full...
      // unsigned long long frequency = SAMPLE_SIZE * 1000000 / (micros() - startTime); // Calculate frequency
      // Serial.printf("Frequency:%lu\n", frequency); // Display frequency
      // startTime = micros(); // reset sampling window timer

      process_samples(); // update processed[] from samples[]
      sample_index = 0; // Reset sample index to overwrite existing values
      processed_data = true;
    }
    if (processed_data)
      writeLED(); // output processed value
    samples[sample_index] = mic_sample; // update samples at sample_index with the most recent microphone sample
    sample_index++; // move to the next index in samples[]
  }
}


// Reads analog value from photoresistor and prints them to serial
void readFromLight() {
  uint16_t light = analogRead(PHOTO_PIN);
  Serial.println(light);
}


// Applies some DSP to samples to update the processed array of samples
void process_samples() {
  // For this case, no DSP is applied
  for(int i = 0; i < SAMPLE_SIZE; i++)
    processed[i] = samples[i];
    
  // For this case, it just computes a moving average with a window of 3 (note: indices SAMPLE_SIZE-2 and SAMPLE_SIZE-1 will remain unchanged)
  // static const int windowSize = 3;
  // for (int i = 0; i + windowSize < SAMPLE_SIZE; i++)
  //   processed[i] = (samples[i] + samples[i+1] + samples[i+2]) / windowSize; // Note: could be done with a for loop, but this method is faster
}


// Outputs processed[sample_index] via light
void writeLED() {
  dacWrite(LED_PIN, processed[sample_index]);
  // Serial.println((uint8_t)processed[sample_index]); // Uncomment to print the value being written to the LED
}


/* Checks whether enough enough time has elapsed to take next microphone sample.
 * 
 * If enough time has elapsed, stores sample into mic_sample and returns true.
 * If not enough time has elapsed, returns false;
 */
bool sampleMic() {
  static int lastSampleTime = micros(); // tracks time of last sample
  int currentTime = micros();
  if (currentTime >= lastSampleTime + period) { // if enough time has elapsed..
    lastSampleTime = currentTime; 
    mic_sample = analogRead(MIC_PIN);
    // Serial.println(mic_sample);
    // Serial.printf("Mic:%d\n", mic_sample); // Uncomment to print mic_sampe to new line on Serial
    return true;
  }
  return false;
}