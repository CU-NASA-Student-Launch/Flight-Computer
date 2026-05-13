#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>
#include <CamControl.hpp>
#include <ServoControl.hpp>
#include <LEDControl.hpp>
#include <GPSControl.hpp>

#include "LittleFS.h"
#include <Wire.h>

BmpControl bmpSensor;
MpuControl mpuSensor;
SpeakerControl speaker;
SensorData currData;
FsWriter writer;
CamControl cam;
ServoControl servo;
LEDControl led;
GPSControl gps;

unsigned long logStart = 40000000;
unsigned long railClearTime = 0;
float initialAltitude = 100000;
bool railCleared = false;
float previousRollRate = 0.0;
float previousTime = 0.0;

void setup()
{
  // Set up usb communication
  Serial.begin(115200);
  delay(3000);

  // Initialize pico pins
  Wire.begin();
  Wire.setClock(400000); // I2C running at 400 kHz

  // Establish connection with sensors
  bmpSensor.connectBmp();
  mpuSensor.connectMpu();
  servo.connectServo();
  gps.connectGPS();

  // Starts infinite loop of silence if file is already there.
  // File will NEVER be overwritten by design.
  // If file is there, begin loop listening for connection over usb.
  writer.checkForFile();

  // Camera on before countdown to power camera for pulling
  // footage between launches.
  cam.setOn();

  // Wait for 5 mins
  for (int i = 0; i < 900; i++)
  {
    // This will cause a fervant sounding series of beeps.
    speaker.beep(0.1);
    delay(100);

    // If little button on board is pressed, jump out of the loop.
    // This is for testing purposes.
    if (BOOTSEL)
    {
      break;
    }
  }

  led.ledsOn();
  // Creates file
  writer.initiate();

  // Board in armed state. Speaker is on all the time and we are waiting
  // for a launch event.
  speaker.blare();
  while (!mpuSensor.checkMotion())
  {
    // Busy wait until an upward acceleration (more negative) is detected.
  }

  // Make note of number of milliseconds from board power-on launch occurs at.
  logStart = millis();
  // Note pre-launch altitude used for servo control
  initialAltitude = bmpSensor.getHeight();
}

void loop()
{
  // Grab data from each of the sensors and store in the currData object.
  bmpSensor.pollBmp(currData);
  mpuSensor.pollMpu(currData);
  gps.pollGPS(currData);
  currData.t_ms = millis(); // Moment in time associated with the data samples

  float angle = 0;
  float rollRate = currData.gyroX;
  float currentPosition = (currData.gyroX + previousRollRate) * 0.5 * (currData.t_ms - previousTime) / 1000.0; // Simple trapezoidal integration to get position from rate.
  previousRollRate = currData.gyroX;
  previousTime = currData.t_ms;

  if (!railCleared && ((currData.altitude - initialAltitude) > 3.5))
  {
    railCleared = true;
    led.leftLedOn();
    servo.adjustAngle(10);
    railClearTime = millis();
  }
  else if (railCleared && (millis() > (1500 + railClearTime)) && !(millis() > (3500 + railClearTime)))
  {
    led.rightLedOn();
    servo.adjustAngle(-20);
  }
  else if (railCleared && (millis() > (3500 + railClearTime)))
  {
    if (abs(rollRate) < 0.1)
    {
      angle = 0.0;
    }
    else if (abs(rollRate) < 0.3)
    {
      angle = 5.0;
    }
    else if (abs(rollRate) < 0.7)
    {
      angle = 10.0;
    }
    else if (abs(rollRate) < 1.4)
    {
      angle = 20;
    }
    else
    {
      angle = 35.0;
    }

    currData.currentPosition = currentPosition;
    currData.angle = -angle;

    float signedAngle = -angle * (rollRate / abs(rollRate));

    if (signedAngle > 0)
    {
      led.rightLedOn();
    }
    else if (signedAngle < 0)
    {
      led.leftLedOn();
    }
    else
    {
      led.ledsOn();
    }

    // Convert from radians to degrees for control signal.
    servo.adjustAngle(signedAngle);
  }

  // Store data in flash memory. This only happens when the buffer is full.
  writer.store(currData);

  constexpr int fifteenSecs = 1000 * 15;  // Fifteen seconds in milliseconds
  constexpr int fiveMins = 1000 * 60 * 5; // Five mins in milliseconds

  // Collect data until fifteen seconds have passed, or until the boot-select
  // button has been pressed. Boot-select option is for debug purposes.
  if (BOOTSEL || (millis() > (fiveMins + logStart)))
  {
    speaker.silence(); // When speaker is quiet, you know logging has stopped.
    cam.setOff();
    servo.adjustAngle(0); // Reset servo to neutral position.
    led.ledsOff();
    writer.flush(); // Clear out what is left in buffer.
    writer.closeFile();
    writer.streamFSData(); // Wait for communication from usb FOREVER.
  }

  // Manual delay to slow down rate of logging. Makes goofy jittery sound because
  // beep is for duration of the delay. This also acts as an audible indicator of state.
  speaker.beep(0.002);
}