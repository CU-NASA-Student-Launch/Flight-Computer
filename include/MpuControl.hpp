#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SensorData.hpp>

#ifndef MPU_CONTROL_HPP
#define MPU_CONTROL_HPP

class MpuControl
{
    public:
        // Constructor
        MpuControl();
        // Member functions
        void connectMpu(void);
        void pollMpu(SensorData &record);
        bool checkTilt(void);
        bool isUpright(void);
        bool checkMotion(void);

    private:
        Adafruit_MPU6050 mpu;
        float x_bias = 0;
        float y_bias = 0;
        float z_bias = 0;
};

#endif // MPU_CONTROL_HPP
