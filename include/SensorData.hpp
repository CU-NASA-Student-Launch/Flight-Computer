// Defining the return types of the pull functions

#ifndef SENSOR_DATA_HPP
#define SENSOR_DATA_HPP

struct MpuData
{
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
};

struct BmpData
{
    float temp;
    float pressure;
    float altitude;
};

#endif // SENSOR_DATA_HPP
