#include <Arduino.h>
#include "FsWriter.hpp"
#include <Record.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>

BmpControl bmpSensor;
MpuControl mpuSensor;

// in here for now. To be changed upon launch event detection
bool recordingEnabled;

void setup() {
    Serial.begin(115200);
    delay(3000);

    // Establish connection with sensors
    bmpSensor.connectBmp();
    mpuSensor.connectMpu();

    recordingEnabled = true;
}

void loop() {
    if (recordingEnabled) { 
        FsWriter buffer;
        Record record;

        for(int i = 0; i < 50; ++i) {
            bmpSensor.pollBmp(record);
            mpuSensor.pollMpu(record);

            buffer.store(record);
            if (buffer.full()) {
                buffer.flush();
            }
            delay(100);
        }

        Serial.println("Done");
        buffer.streamFSData();
        recordingEnabled = false;
    }
}