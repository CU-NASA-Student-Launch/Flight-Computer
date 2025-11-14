#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SensorData.hpp>

class MpuControl
{
    public:
        // Constructor
        MpuControl();
        // Member functions
        void connectMpu(void);
        MpuData pollMpu(void);


    private:
        Adafruit_MPU6050 mpu;
};