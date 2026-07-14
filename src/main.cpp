#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <arduinoFFT.h>

#define SAMPLE_RATE 2000
#define SAMPLES 1024
#define SERIAL_BAUD 921600 

float vRealX[SAMPLES], vImagX[SAMPLES];
float vRealY[SAMPLES], vImagY[SAMPLES];
float vRealZ[SAMPLES], vImagZ[SAMPLES];

// Time-domain backup arrays
float timeX[SAMPLES], timeY[SAMPLES], timeZ[SAMPLES];

ArduinoFFT<float> FFT_X = ArduinoFFT<float>(vRealX, vImagX, SAMPLES, SAMPLE_RATE);
ArduinoFFT<float> FFT_Y = ArduinoFFT<float>(vRealY, vImagY, SAMPLES, SAMPLE_RATE);
ArduinoFFT<float> FFT_Z = ArduinoFFT<float>(vRealZ, vImagZ, SAMPLES, SAMPLE_RATE);

const int sampleTime = round(1000000 * (1.0 / SAMPLE_RATE)); // Sample time in microseconds

Adafruit_MPU6050 mpu;

void collectSamples(float *meanX, float *meanY, float *meanZ);
void streamData(void);
void executeFFT(float meanX, float meanY, float meanZ);

void setup(void) {
  Serial.begin(SERIAL_BAUD);
  delay(500); 
  Serial.println("\n=== System Initializing ===");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); 
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);

  delay(100);
}

void loop() {
  float meanX=0, meanY=0, meanZ=0;
  collectSamples(&meanX, &meanY, &meanZ);
  executeFFT(meanX, meanY, meanZ);
  streamData();
  delay(30); 
}

void collectSamples(float *meanX, float *meanY, float *meanZ){
  sensors_event_t a, g, temp; 
  float sumX = 0, sumY = 0, sumZ = 0;
  unsigned long prevTime = micros(); 
  int i = 0;
  
  while(i < SAMPLES){
    unsigned long startTime = micros();
    if((startTime - prevTime) >= sampleTime){
    
      mpu.getEvent(&a, &g, &temp); 
  
      timeX[i] = a.acceleration.x;
      timeY[i] = a.acceleration.y;
      timeZ[i] = a.acceleration.z;

      vRealX[i] = a.acceleration.x; vImagX[i] = 0;
      vRealY[i] = a.acceleration.y; vImagY[i] = 0;
      vRealZ[i] = a.acceleration.z; vImagZ[i] = 0;

      sumX += vRealX[i]; sumY += vRealY[i]; sumZ += vRealZ[i];
      
      prevTime = startTime; 
      i++;
    }
    yield();
  }
  *meanX = sumX / SAMPLES;
  *meanY = sumY / SAMPLES;
  *meanZ = sumZ / SAMPLES;
}

void executeFFT(float meanX, float meanY, float meanZ){
  for(int i = 0; i < SAMPLES; i++) {
    vRealX[i] -= meanX; vRealY[i] -= meanY; vRealZ[i] -= meanZ;
  }

  FFT_X.windowing(FFTWindow::Hamming, FFTDirection::Forward); 
  FFT_Y.windowing(FFTWindow::Hamming, FFTDirection::Forward); 
  FFT_Z.windowing(FFTWindow::Hamming, FFTDirection::Forward); 

  FFT_X.compute(FFTDirection::Forward);
  FFT_Y.compute(FFTDirection::Forward);
  FFT_Z.compute(FFTDirection::Forward);

  FFT_X.complexToMagnitude();
  FFT_Y.complexToMagnitude();
  FFT_Z.complexToMagnitude();
}


void streamData(void) {
  for (int i = 0; i < SAMPLES; i++) { Serial.print(timeX[i], 1); if (i < SAMPLES - 1) Serial.print(","); }
  Serial.print(";");
  for (int i = 0; i < SAMPLES; i++) { Serial.print(timeY[i], 1); if (i < SAMPLES - 1) Serial.print(","); }
  Serial.print(";");
  for (int i = 0; i < SAMPLES; i++) { Serial.print(timeZ[i], 1); if (i < SAMPLES - 1) Serial.print(","); }

  Serial.print("|"); 

  for (int i = 0; i < 512; i++) { Serial.print(vRealX[i], 1); if (i < 511) Serial.print(","); }
  Serial.print(";");
  for (int i = 0; i < 512; i++) { Serial.print(vRealY[i], 1); if (i < 511) Serial.print(","); }
  Serial.print(";");
  for (int i = 0; i < 512; i++) { Serial.print(vRealZ[i], 1); if (i < 511) Serial.print(","); }
  Serial.println(); 
}