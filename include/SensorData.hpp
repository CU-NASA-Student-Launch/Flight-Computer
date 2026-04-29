// Defining the return types of the poll functions

#ifndef SENSOR_DATA_HPP
#define SENSOR_DATA_HPP

struct __attribute__((packed)) SensorData
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
    float currentPosition;
    float angle;
    float gps_alt;
    float gps_lat;
    float gps_lon;
};

#endif // SENSOR_DATA_HPP
