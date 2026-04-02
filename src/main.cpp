#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>

#include "DC_Motor.hpp"
#include "BrakeController.hpp"

#include "LittleFS.h"

#include <Wire.h> // This is for the specification which pins to use for I2C


// ── Pin assignments for DC Motor ────────────────────────────────────────────
constexpr int PIN_IN1     = 13;   // DRV8872 IN1  — must be PWM-capable
constexpr int PIN_IN2     = 12;   // DRV8872 IN2  — must be PWM-capable
constexpr int PIN_ENDSTOP = 24;   // Limit switch (INPUT_PULLUP)
constexpr int PIN_ENC_A   = 11;   // Encoder A    — interrupt-capable
constexpr int PIN_ENC_B   = 10;   // Encoder B    — interrupt-capable

// Motor Constants
constexpr float GEAR_RATIO   = 30.0f;
constexpr int   CPR          = 64;

// ── Global objects ───────────────────────────────────────────────────────

#define BURNOUT_MASS 9.958
#define BURNOUT_TIME 3063 // in ms
#define PROJECTED_AREA 0.008171 // frontal area of rocket
#define SAMPLING_RATE 10 // Hz
#define INITIAL_MAGIC_ANGLE 20.0f // degrees 

BrakeController* brakeController = nullptr;

BmpControl bmpSensor;
MpuControl mpuSensor;
SpeakerControl speaker;
DC_Motor motor(PIN_IN1, PIN_IN2, PIN_ENDSTOP, PIN_ENC_A, PIN_ENC_B, GEAR_RATIO, CPR);
SensorData currData;
FsWriter writer;



int logStart = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);

    // specifying I2C pins
    Wire.begin();

    // Initialize the motor (blocks until homed)
    motor.begin();

    // Establish connection with sensors
    bmpSensor.connectBmp();
    mpuSensor.connectMpu();

    writer.initiate(); // loops forever if file is already there

    // // Wait for rocket to be turned upside down for five seconds
    // // before starting logging.
    // while(!mpuSensor.checkUpsideDown())
    // {
    //     speaker.beep(1);
    //     delay(1000);
    //     speaker.beep(1);
    // }
    
    // Wait for 5 mins after power on
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

    float now = millis();
    bmpSensor.pollBmp(currData);
    mpuSensor.pollMpu(currData);
    currData.t_ms = millis();
    writer.store(currData);

    constexpr int tenMinMilli = 1000*60*60; // Thirty minutes in milliseconds

    // State 1: Detect motor burnout & Ramp to initial magic angle for 2 seconds after burnout

    if(now > BURNOUT_TIME) {
        if(brakeController == nullptr) {
            brakeController = new BrakeController(BURNOUT_MASS, BURNOUT_TIME, PROJECTED_AREA, SAMPLING_RATE);
        }
        motor.setFlapTarget(INITIAL_MAGIC_ANGLE);
    }

    // State 3: Once initial angle is acheived, calculate optimal deployment 

    

    // State 4: Learn CD profile in real time and update brake controller's internal model

    // State 5: Detect apogee and retract airbrakes for landing

    if(BOOTSEL || (millis() > (tenMinMilli + logStart)))
    {
        speaker.silence();
        writer.streamFSData();
    }

    speaker.beep(0.031);
}
