#include <Arduino.h>
#include "SampleBuffer.h"
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>

BmpControl bmpSensor;
MpuControl mpuSensor;

void setup() {
    // Serial.begin(115200);
    // delay(3000);
    // SampleBuffer buffer;

    // for(int i = 0; i < 50; ++i) {
    //     buffer.Collect();
    //     if (buffer.Full()) {
    //         buffer.Flush();
    //     }
    //     delay(100);
    // }

    // Serial.println("Done");
    // buffer.StreamFSData();

    // Set up serial connection
    
    Serial.begin(115200);
    delay(3000);

    // Establish connection with sensors
    bmpSensor.connectBmp();
    mpuSensor.connectMpu();
}

void loop() {
    BmpData polledBmpData = bmpSensor.pollBmp();
    MpuData polledMpuData = mpuSensor.pollMpu();

    // Print BMP data
    Serial.print(F("Temperature = "));
    Serial.print(polledBmpData.temp);
    Serial.println(" Celsius");

    Serial.print(F("Pressure = "));
    Serial.print(polledBmpData.pressure);
    Serial.println(" Pa");

    Serial.print(F("Altitude = "));
    Serial.print(polledBmpData.altitude);
    Serial.println(" m");
    Serial.println();

    Serial.print(F("AccelX = "));
    Serial.print(polledMpuData.accelX);
    Serial.println(" m/s");

    Serial.print(F("AccelY = "));
    Serial.print(polledMpuData.accelY);
    Serial.println(" m/s");

    Serial.print(F("AccelZ = "));
    Serial.print(polledMpuData.accelZ);
    Serial.println(" m/s");

    Serial.print(F("GyroX = "));
    Serial.print(polledMpuData.gyroX);
    Serial.println(" rad/s");

    Serial.print(F("GyroY = "));
    Serial.print(polledMpuData.gyroY);
    Serial.println(" rad/s");
    
    Serial.print(F("GyroZ = "));
    Serial.print(polledMpuData.gyroZ);
    Serial.println(" rad/s");
    Serial.println();

    delay(2000);
}