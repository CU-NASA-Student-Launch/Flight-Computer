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
    float latitude;
    float longitude;
    float gpsAltitude;
};

#endif // SENSOR_DATA_HPP
