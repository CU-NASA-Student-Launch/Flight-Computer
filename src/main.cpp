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

    // Wait for rocket to be turned upside down for five seconds
    // before starting logging.
    while(!mpuSensor.checkUpsideDown())
    {
        recordingEnabled = true;
    }
    
    delay(300000); // Wait for 5 mins after upside-down event
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
