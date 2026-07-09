#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define SAMPLE_RATE 2000
#define SAMPLES 1024

float vRealX[SAMPLES];
float vImagX[SAMPLES];
float vRealY[SAMPLES];
float vImagY[SAMPLES];
float vRealZ[SAMPLES];
float vImagZ[SAMPLES];

const int sampleTime = round(1000000 * (1.0 / SAMPLE_RATE)); // Sample time in microseconds

Adafruit_MPU6050 mpu;

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

void getData(void);
void samplingWindow(void);
void displayData(void);

void setup(void) {
  Serial.begin(9600);
  delay(500); 
  Serial.println("\n=== System Initializing ===");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);

  delay(100);
}

void loop() {
  samplingWindow();
  displayData();
  delay(1000);
}


void getData(void) {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp); 
  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
  gyroX = g.gyro.x;
  gyroY = g.gyro.y;
  gyroZ = g.gyro.z;
}

void samplingWindow(void){
  unsigned long prevTime=micros(); 
  int i=0;
  while(i<SAMPLES){
    unsigned long startTime = micros();
    if((startTime-prevTime)>=sampleTime){
      getData();
      vRealX[i] = accX;
      vImagX[i] = 0;
      vRealY[i] = accY;
      vImagY[i] = 0;
      vRealZ[i] = accZ;
      vImagZ[i] = 0;
      prevTime=startTime; 
      i++;
    }
    yield();
  }
 }

 void displayData(void){
for(int i = 0; i < SAMPLES; i++) {
    Serial.printf("Sample [%d] -> X: %.2f | Y: %.2f | Z: %.2f\n", i, vRealX[i], vRealY[i], vRealZ[i]);
    delay(500);
  } 
}
