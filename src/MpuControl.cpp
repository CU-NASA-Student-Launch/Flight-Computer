#include <MpuControl.hpp>

MpuControl::MpuControl()
{
  // Do nothing
}

void MpuControl::connectMpu(void) 
{
  // Find MPU6050
  if (!mpu.begin()) 
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1) 
    {
      delay(10); // Stall forever if no MPU6050 is found
    }
  }
  Serial.println("MPU6050 Found!");

  // Setup motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(false);
  mpu.setMotionInterrupt(true);
}

void MpuControl::pollMpu(Record &record) 
{
  // Get new sensor events with the readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  record.accelX = a.acceleration.x;
  record.accelY = a.acceleration.y;
  record.accelZ = a.acceleration.z;
  record.gyroX = g.gyro.x;
  record.gyroY = g.gyro.y;
  record.gyroZ = g.gyro.z;
}