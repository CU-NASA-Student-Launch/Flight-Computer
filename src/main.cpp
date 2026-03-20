#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>

#include "LittleFS.h"

#include <Wire.h> // This is for the specification which pins to use for I2C

BmpControl bmpSensor;
MpuControl mpuSensor;
SpeakerControl speaker;
SensorData currData;
FsWriter writer;

unsigned long logStart = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);

    // specifying I2C pins
    Wire.begin();

    // Establish connection with sensors
    bmpSensor.connectBmp();
    mpuSensor.connectMpu();

    writer.initiate(); // loops forever if file is already there

    // Wait for rocket to be turned slightly past horizontal
    // before starting logging.
    while(!mpuSensor.checkActivationAngle())
    {
        speaker.beep(1);
        delay(1000);
        speaker.beep(1);
    }
    
    // Wait for 5 mins after upside-down event
    for(int i = 0; i < 1500; i++)
    {
        // Beep fervantly
        speaker.beep(0.1);
        delay(100);

        if(BOOTSEL)
        {
            break;
        }
    }

    speaker.blare();
    while(!mpuSensor.checkMotion());
    logStart = millis();
}

void loop() {
    bmpSensor.pollBmp(currData);
    mpuSensor.pollMpu(currData);
    currData.t_ms = millis() - logStart;
    writer.store(currData);

    constexpr unsigned long duration = 1000*60*60; // 1 hour in milliseconds

    if(BOOTSEL || (millis() > (duration + logStart)))
    {
        speaker.silence();
        writer.streamFSData();
    }

    speaker.beep(0.031);
}
