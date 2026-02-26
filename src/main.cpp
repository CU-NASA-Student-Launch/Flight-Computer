#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>
#include <FanControl.hpp>
#include <CamControl.hpp>

#include "LittleFS.h"

#include <Wire.h> // This is for the specification which pins to use for I2C

//BmpControl bmpSensor;
MpuControl mpuSensor;
SpeakerControl speaker;
SensorData currData;
FsWriter writer;
FanControl fan;
CamControl cam;

int logStart = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);

    // specifying I2C pins
    Wire.begin();

    // Establish connection with sensors
    //bmpSensor.connectBmp();
    mpuSensor.connectMpu();

    fan.setOn();

    writer.initiate(); // loops forever if file is already there

    // Wait for rocket to be turned upside down for five seconds
    // before starting logging.
    // while(!mpuSensor.checkTilt())
    // {
    //     speaker.beep(1);
    //     delay(1000);
    //     speaker.beep(1);
    // }
    
    // Wait for 3 mins after tilt event
    for(int i = 0; i < 900; i++)
    {
        // Beep fervantly
        speaker.beep(0.1);
        delay(100);

        if(BOOTSEL)
        {
            break;
        }
    }

    cam.setOn();

    // Wait for 2 mins after giving camera power
    for(int i = 0; i < 600; i++)
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
    fan.setOff();
}

void loop() {
    //bmpSensor.pollBmp(currData);
    mpuSensor.pollMpu(currData);
    currData.t_ms = millis();
    writer.store(currData);

    constexpr int tenMins = 1000*60*10; // Ten minutes in milliseconds
    constexpr int fifteenSecs = 1000*15; // Fifteen seconds in milliseconds

    if(BOOTSEL || (millis() > (tenMins + logStart)))
    {
        speaker.silence();
        fan.setOn();
        writer.streamFSData();
    }

    // Turn the fan 30 seconds after launch
    if(millis() > (fifteenSecs + logStart))
    {
        fan.setOn();
    }

    speaker.beep(0.031);
}
