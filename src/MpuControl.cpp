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
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  }
}

bool MpuControl::checkUpsideDown(void)
{
  sensors_event_t a, g;
  mpu.getEvent(&a, &g, nullptr);

  bool upsideDown = true;
  
  // Check once a second for five seconds
  // If ever right-side up, we haven't been upside-down for long enough
  for(int i = 0; i < 100; i++)
  {
    if(a.acceleration.y + y_bias > -2 || a.acceleration.y + y_bias < -7)
    {
      upsideDown = false;
    }
    mpu.getEvent(&a, &g, nullptr);
    delay(50);
  }
  return upsideDown;
}

bool MpuControl::checkMotion(void)
{
  sensors_event_t a;
  mpu.getEvent(&a, nullptr, nullptr);
  return a.acceleration.x < -10; // negative x is up
}

void MpuControl::pollMpu(SensorData &record) 
{
  // Get new sensor events with the readings
  sensors_event_t a, g;
  mpu.getEvent(&a, &g, nullptr);

  record.accelX = a.acceleration.x + x_bias;
  record.accelY = a.acceleration.y + y_bias;
  record.accelZ = a.acceleration.z + z_bias;
  record.gyroX = g.gyro.x;
  record.gyroY = g.gyro.y;
  record.gyroZ = g.gyro.z;
}
