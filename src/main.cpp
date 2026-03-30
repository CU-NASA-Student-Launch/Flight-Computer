#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>
#include <FanControl.hpp>
#include <CamControl.hpp>
#include <ServoControl.hpp>

#include "LittleFS.h"
#include <Wire.h>

BmpControl bmpSensor;
MpuControl mpuSensor;
SpeakerControl speaker;
SensorData currData;
FsWriter writer;
FanControl fan;
CamControl cam;
ServoControl servo;

unsigned long logStart = 40000000; // Not a permenant solution
unsigned long railClearTime = 0;
float initialAltitude = 100000;
bool railCleared = false;

void setup() {
    // Set up usb communication
    Serial.begin(115200);
    delay(3000);

    // Initialize pico pins
    Wire.begin();
    Wire.setClock(400000);  // I2C running at 400 kHz

    // Establish connection with sensors
    bmpSensor.connectBmp();
    mpuSensor.connectMpu();
    servo.connectServo();

    // Starts infinite loop of silence if file is already there.
    // File will NEVER be overwritten by design.
    // If file is there, begin loop listening for connection over usb.
    writer.checkForFile();
    
    // Wait for 3 mins after tilt event to give time to place rocket on rail.
    for(int i = 0; i < 900; i++)
    {
        // Beep fervantly
        speaker.beep(0.1);
        delay(100);

        // If little button on board is pressed, jump out of the loop.
        // This is for testing purposes.
        if(BOOTSEL)
        {
            break;
        }
    }

    // Initializes filesystem and creates file after check for preexisting file.
    writer.initiate();

    // Turn fan on before camera to prevent overheating.
    fan.setOn();
    // Turn the camera on, then give a few minutes to make sure camera is connected.
    cam.setOn();

    // Board in armed state. Speaker is on all the time and we are waiting
    // for a launch event.
    speaker.blare();
    while(!mpuSensor.checkMotion());

    // Make of note of the moment of launch.
    // Millis gives us the time from power on of the board in milliseconds.
    logStart = millis();
    // Initial height used for servo control
    initialAltitude = bmpSensor.getHeight();
    // Turn the fan on at launch to prevent interferance in flight.
    fan.setOff();
}

void loop() {
    // Grab data from each of the sensors and store in the currData object.
    bmpSensor.pollBmp(currData);
    mpuSensor.pollMpu(currData);
    currData.t_ms = millis(); // Moment in time associated with the data samples
    // Store data in flash memory. This only happens if the buffer is full.
    // This check is done in the store method.

    if(!railCleared && ((currData.altitude - initialAltitude) > 3.5))
    {
        railCleared = true;
        servo.adjustAngle(45);
        railClearTime = millis();
    }
    else if(railCleared && (millis() > (2000 + railClearTime)))
    {
        servo.adjustAngle(-45);
    }

    writer.store(currData);

    constexpr int fifteenSecs = 1000*15; // Fifteen seconds in milliseconds
    constexpr int fiveMins = 1000*60*5; // Five mins in milliseconds

    // Got data for 15 secs after launch event. For logging purposes, if little
    // button on the board is pressed, stop logging early.
    if(BOOTSEL || (millis() > (fiveMins + logStart)))
    {
        speaker.silence(); // When speaker is quiet, you know logging has stopped.
        cam.setOff();
        fan.setOff(); // Fan must remain on while camera is on always to prevent damage.
        writer.flush();
        writer.closeFile();
        writer.streamFSData();
    }

    if(millis() > (fifteenSecs + logStart))
    {
        fan.setOn(); // Fan must remain on while camera is on always to prevent damage.
    }

    // Manual delay to slow down rate of logging. Makes goofy jittery sound because 
    // beep is for duration of the delay.
    speaker.beep(0.002);
}
