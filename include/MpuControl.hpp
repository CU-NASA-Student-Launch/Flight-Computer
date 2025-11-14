#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class MpuControl
{
    public:
        // Member functions
        void connectMpu(void);
        void pollMpu(void);


    private:
        Adafruit_MPU6050 mpu;
};