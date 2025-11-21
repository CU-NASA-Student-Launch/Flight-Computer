#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>

#include "LittleFS.h"

BmpControl bmpSensor;
MpuControl mpuSensor;
SpeakerControl speaker;
SensorData currData;
FsWriter writer;

int logStart;

void setup() {
    Serial.begin(115200);
    delay(3000);
    if (!LittleFS.exists("/flight.csv")) {
        Serial.println(F("NO_FILE_AFTER_HEADER_WRITE"));
    }

    // Establish connection with sensors
    bmpSensor.connectBmp();
    mpuSensor.connectMpu();

    writer.initiate(); // loops forever if file is already there
    
    // Wait for rocket to be turned upside down for five seconds
    // before starting logging.
    while(!mpuSensor.checkUpsideDown())
    {
        speaker.beep(1);
        delay(1000);
        speaker.beep(1);
    }
    
    // Wait for 5 mins after upside-down event
    for(int i = 0; i < 30; i++)
    {
        // Beep fervantly
        speaker.beep(0.1);
        delay(100);
    }

    speaker.blare();
    while(!mpuSensor.checkMotion());
    logStart = millis();
}

void loop() {
    bmpSensor.pollBmp(currData);
    mpuSensor.pollMpu(currData);
    currData.t_ms = millis();
    writer.store(currData);

    constexpr int tenMinMilli = 1000*60*10; // Ten minutes in milliseconds

    if(BOOTSEL || (millis() > (tenMinMilli + logStart)))
    {
        speaker.silence();
        writer.streamFSData();
        for(;;);
    }

    speaker.beep(0.031);
}
