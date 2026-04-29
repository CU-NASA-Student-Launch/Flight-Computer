#include <Arduino.h>
#include <TinyGPS++.h>
#include <SensorData.hpp>

#ifndef GPS_CONTROL_HPP
#define GPS_CONTROL_HPP

class GPSControl
{
public:
    GPSControl();
    void connectGPS(void);
    void pollGPS(SensorData &record);

private:
    TinyGPSPlus gps;
    unsigned long lastPrint = 0;
};

#endif // GPS_CONTROLL_HPP