#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

void getData();

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
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}

void loop() {
  getData(); 
  delay(500); 
}


void getData() {
  
  sensors_event_t a, g, temp;
  
  mpu.getEvent(&a, &g, &temp); 

  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
  
  gyroX = g.gyro.x;
  gyroY = g.gyro.y;
  gyroZ = g.gyro.z;

  Serial.print("Acceleration X: ");   
  Serial.print(accX);
  Serial.print(", Y: ");              
  Serial.print(accY);
  Serial.print(", Z: ");              
  Serial.print(accZ);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");       
  Serial.print(gyroX);
  Serial.print(", Y: ");              
  Serial.print(gyroY);
  Serial.print(", Z: ");              
  Serial.print(gyroZ);
  Serial.println(" rad/s");
  Serial.println("──────────────────────────────────────────────────");
}