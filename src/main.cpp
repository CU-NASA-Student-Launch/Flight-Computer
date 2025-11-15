#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
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
        SensorData currData;
        FsWriter writer;

        // Right now we are only logging 70 items in the csv
        for(int i = 0; i < 70; i++) {
            bmpSensor.pollBmp(currData);
            mpuSensor.pollMpu(currData);
            currData.t_ms = millis();
            writer.store(currData);
        }

        writer.flush();

        Serial.println("Done");
        writer.streamFSData();
        recordingEnabled = false;
    }
}
