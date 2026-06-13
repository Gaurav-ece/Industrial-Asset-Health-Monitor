#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Global variables to store the data
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

void getData();

void setup(void) {
  Serial.begin(9600);
  
  // Small stabilization delay to give the hardware serial lines time to clear
  delay(500); 
  Serial.println("\n=== System Initializing ===");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // Stable configuration settings
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}

void loop() {
  getData(); 
  delay(500); // 500ms delay keeps the terminal highly readable for this test
}


void getData() {
  // Create local containers for all 3 data types
  sensors_event_t a, g, temp;
  
  // Pass the actual 'temp' address instead of NULL to prevent the library from crashing
  mpu.getEvent(&a, &g, &temp); 

  /* Safe assignment to your global parameters */
  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
  
  gyroX = g.gyro.x;
  gyroY = g.gyro.y;
  gyroZ = g.gyro.z;

  /* Print out the values cleanly */
  Serial.print("Acceleration X: ");   Serial.print(accX);
  Serial.print(", Y: ");              Serial.print(accY);
  Serial.print(", Z: ");              Serial.print(accZ);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");       Serial.print(gyroX);
  Serial.print(", Y: ");              Serial.print(gyroY);
  Serial.print(", Z: ");              Serial.print(gyroZ);
  Serial.println(" rad/s");
  Serial.println("──────────────────────────────────────────────────");
}