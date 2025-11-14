#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Record.hpp>

#ifndef MPU_CONTROL_HPP
#define MPU_CONTROL_HPP

class MpuControl
{
    public:
        // Constructor
        MpuControl();
        // Member functions
        void connectMpu(void);
        void pollMpu(Record &record);


    private:
        Adafruit_MPU6050 mpu;
};

#endif // MPU_CONTROL_HPP