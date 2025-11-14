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
  }

  // Setup motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(false);
  mpu.setMotionInterrupt(true);
}

MpuData MpuControl::pollMpu() 
{
  // Get new sensor events with the readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  MpuData polledMpuData;

  polledMpuData.accelX = a.acceleration.x;
  polledMpuData.accelY = a.acceleration.y;
  polledMpuData.accelZ = a.acceleration.z;
  polledMpuData.gyroX = g.gyro.x;
  polledMpuData.gyroY = g.gyro.y;
  polledMpuData.gyroZ = g.gyro.z;

  return polledMpuData;
}