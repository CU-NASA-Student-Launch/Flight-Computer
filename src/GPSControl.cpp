#include <GPSControl.hpp>

GPSControl::GPSControl()
{
    // Do nothing
}

bool GPSControl::hasFix()
{
    return gps.location.isValid();
}


void GPSControl::connectGPS(void)
{
    Serial.begin(115200);

    Serial1.setTX(0);
    Serial1.setRX(1);
    Serial1.begin(9600);

    // Check if GPS has connected to UART or not
    unsigned long start = millis();
    while (millis() - start < 5000)
    {
        while (Serial1.available())
        {
            gps.encode(Serial1.read());
        }

        if (gps.charsProcessed() > 10)
        {
            Serial.println("GPS detected.");
            return;
        }
    }
    Serial.println("ERROR: No GPS data detected.");
}

void GPSControl::pollGPS(SensorData &record)
{
    while (Serial1.available())
    {
        char c = Serial1.read();
        gps.encode(c);
    }

    // Normal GPS logic
    if (gps.location.isUpdated())
    {
        record.gps_lat = gps.location.lat();
        record.gps_lon = gps.location.lng();
        record.gps_alt = gps.altitude.meters();
    }
}