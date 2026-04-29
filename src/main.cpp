#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <GPSControl.hpp>
#include <SpeakerControl.hpp>
#include <FanControl.hpp>
#include <CamControl.hpp>
#include <ServoControl.hpp>

#include "LittleFS.h"
#include <Wire.h>

BmpControl bmpSensor;
MpuControl mpuSensor;
GPSControl gpsSensor;
SpeakerControl speaker;
SensorData currData;
FsWriter writer;
FanControl fan;
CamControl cam;
ServoControl servo;

unsigned long logStart = 0;

void setup() {
    // Set up usb communication
    Serial.begin(115200);
    delay(3000);

    // Initialize pico pins
    Wire.begin();

    // Establish connection with sensors
    bmpSensor.connectBmp();
    mpuSensor.connectMpu();
    gpsSensor.connectGPS();
    servo.connectServo();

    // Starts infinite loop of silence if file is already there.
    // File will NEVER be overwritten by design.
    // If file is there, begin loop listening for connection over usb.
    writer.checkForFile();

    // Wait for rocket to be held at ~45° nose down for five seconds
    while(!mpuSensor.checkTilt())
    {
        servo.adjustAngle(40);
        //speaker.beep(1);
        delay(1000);
        servo.adjustAngle(0);
        //speaker.beep(1);
    }

    // Initializes filesystem and creates file after check for preexisting file.
    writer.initiate();
    
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

    // Turn fan on before camera to prevent overheating.
    fan.setOn();

    // Turn the camera on, then give a few minutes to make sure camera is connected.
    cam.setOn();

    // Wait for 2 mins after giving camera power
    for(int i = 0; i < 600; i++)
    {
        // Beep fervantly
        speaker.beep(0.1);
        delay(100);

        // Press little button on board to skip wait for testing.
        if(BOOTSEL)
        {
            break;
        }
    }
    
    // Board in armed state. Speaker is on all the time and we are waiting
    // for a launch event.
    speaker.blare();
    while(!mpuSensor.checkMotion());

    // Make of note of the moment of launch.
    // Millis gives us the time from power on of the board in milliseconds.
    logStart = millis();
    // Turn the fan on at launch to prevent interferance in flight.
    fan.setOff();
}

void loop() {
    // Grab data from each of the sensors and store in the currData object.
    bmpSensor.pollBmp(currData);
    mpuSensor.pollMpu(currData);
    gpsSensor.pollGPS(currData);
    currData.t_ms = millis(); // Moment in time associated with the data samples
    // Store data in flash memory. This only happens if the buffer is full.
    // This check is done in the store method.
    writer.store(currData);

    constexpr int tenMins = 1000*60*10; // Ten minutes in milliseconds
    constexpr int fifteenSecs = 1000*15; // Fifteen seconds in milliseconds

    // Got data for 10 mins after launch event. For logging purposes, if little
    // button on the board is pressed, stop logging early.
    if(BOOTSEL || (millis() > (tenMins + logStart)))
    {
        speaker.silence(); // When speaker is quiet, you know logging has stopped.
        fan.setOn(); // Fan must remain on while camera is on always to prevent damage.
        writer.streamFSData(); // Enter loop that listens for usb connection.
    }

    // Turn the fan on 15 seconds after launch.
    // Fan MUST remain on when camera is on to prevent damage!
    if(millis() > (fifteenSecs + logStart))
    {
        fan.setOn();
    }

    // Manual delay to slow down rate of logging. Makes goofy jittery sound because 
    // beep is for duration of the delay.
    speaker.beep(0.001);
}
