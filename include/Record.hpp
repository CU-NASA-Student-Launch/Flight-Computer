// Defining the return types of the poll functions

#ifndef RECORD_HPP
#define RECORD_HPP

struct Record
{
    uint32_t t_ms;
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float temp;
    float pressure;
    float altitude;
};

#endif // RECORD_HPP
