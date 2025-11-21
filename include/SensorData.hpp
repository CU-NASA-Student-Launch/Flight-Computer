// Defining the return types of the poll functions

#ifndef SENSOR_DATA_HPP
#define SENSOR_DATA_HPP

struct SensorData
{
    uint32_t t_ms;
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float pressure;
    float altitude;
};

#endif // SENSOR_DATA_HPP
