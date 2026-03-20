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
  else
  {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  }
}

bool MpuControl::checkTilt(void)
{
  sensors_event_t a, g;
  mpu.getEvent(&a, &g, nullptr);

  bool tilted = true;
  
  // Check once a second for five seconds
  // If ever right-side up, we haven't been upside-down for long enough
  for(int i = 0; i < 100; i++)
  {
    if(a.acceleration.x < 2 || a.acceleration.x > 8)
    {
      tilted = false;
    }
    mpu.getEvent(&a, &g, nullptr);
    delay(50);
  }
  return tilted;
}

bool MpuControl::checkMotion(void)
{
  sensors_event_t a;
  mpu.getEvent(&a, nullptr, nullptr);
  return a.acceleration.x > -9;
}

void MpuControl::pollMpu(SensorData &record) 
{
  // Get new sensor events with the readings
  sensors_event_t a, g;
  mpu.getEvent(&a, &g, nullptr);

  record.accelX = a.acceleration.x;
  record.accelY = a.acceleration.y;
  record.accelZ = a.acceleration.z;
  record.gyroX = g.gyro.x;
  record.gyroY = g.gyro.y;
  record.gyroZ = g.gyro.z;
}
