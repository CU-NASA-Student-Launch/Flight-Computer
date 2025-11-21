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
        bool checkUpsideDown(void);
        bool checkMotion(void);
        static void resetMoved();

    private:
        Adafruit_MPU6050 mpu;
        const int mpuIntPin = 17;
        static void setMoved();
};

#endif // MPU_CONTROL_HPP
