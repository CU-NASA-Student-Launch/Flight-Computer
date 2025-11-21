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
}

bool MpuControl::checkUpsideDown(void)
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  bool upsideDown = true;
  // Check once a second for five seconds
  // If ever right-side up, we haven't been upside-down for long enough
  for(int i = 0; i < 5; i++)
  {
    if(a.acceleration.y > 8)
    {
      upsideDown = false;
    }
    delay(500);
  }
  return upsideDown;
}

bool MpuControl::checkMotion(void)
{
  sensors_event_t a;
  mpu.getEvent(&a, nullptr, nullptr);
  return a.acceleration.y > 11;
}

void MpuControl::pollMpu(SensorData &record) 
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
