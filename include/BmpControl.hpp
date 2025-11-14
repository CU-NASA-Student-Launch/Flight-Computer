#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <SensorData.hpp>

#define BMP280_ADDRESS_ALT 0x76
#define BMP280_CHIPID 0x58

class BmpControl
{
public:
    BmpControl();
    void connectBmp(void);
    BmpData pollBmp(void);

private:
    Adafruit_BMP280 bmp; // use I2C interface
    Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
    Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
};