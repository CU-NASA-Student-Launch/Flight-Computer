#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>

BmpControl bmpSensor;
MpuControl mpuSensor;
SpeakerControl speaker;

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
        speaker.beep(1);
        delay(1000);
        speaker.beep(1);
    }

    speaker.beep(0.1);
    delay(100);
    speaker.beep(0.1);
    delay(100);
    speaker.beep(0.1);
    delay(100);
    
    // Wait for 5 mins after upside-down event
    for(int i = 0; i < 3000; i++)
    {
        // Beep fervantly
        speaker.beep(0.1);
        delay(100); 
    }

    // Add beeping to this too
    mpuSensor.stallUntilMotion();
}

void loop() {
    SensorData currData;
    //FsWriter writer;

    // Right now we are only logging 70 items in the csv
    for(int i = 0; i < 70; i++) {
        bmpSensor.pollBmp(currData);
        mpuSensor.pollMpu(currData);
        currData.t_ms = millis();
        //writer.store(currData);
    }

    //writer.flush();

    Serial.println("Done");
    //writer.streamFSData();
}
