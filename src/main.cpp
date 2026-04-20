#include <Arduino.h>
#include <FsWriter.hpp>
#include <SensorData.hpp>
#include <BmpControl.hpp>
#include <MpuControl.hpp>
#include <SpeakerControl.hpp>
#include <FanControl.hpp>
#include <CamControl.hpp>
#include <ServoControl.hpp>
#include <LEDControl.hpp>

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
LEDControl led;

unsigned long logStart = 40000000;
unsigned long railClearTime = 0;
float initialAltitude = 100000;
bool railCleared = false;

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

  // Starts infinite loop of silence if file is already there.
  // File will NEVER be overwritten by design.
  // If file is there, begin loop listening for connection over usb.
  writer.checkForFile();

  // Turn fan and camera on before countdown to power camera for pulling
  // footage between launches.
  fan.setOn();
  cam.setOn();

  // Wait for 5 mins
  for (int i = 0; i < 1500; i++)
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

  // Initializes filesystem and creates file after check for preexisting file.
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
  // Turn the fan off at launch to prevent interference during ascent.
  fan.setOff();
}

void loop()
{
  // Grab data from each of the sensors and store in the currData object.
  bmpSensor.pollBmp(currData);
  mpuSensor.pollMpu(currData);
  currData.t_ms = millis(); // Moment in time associated with the data samples

  if (!railCleared && ((currData.altitude - initialAltitude) > 3.5))
  {
    railCleared = true;
    led.leftLedOn();
    servo.adjustAngle(10);
    railClearTime = millis();
  }
  else if (railCleared && (millis() > (1000 + railClearTime)))
  {
    led.rightLedOn();
    servo.adjustAngle(-20);
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
    fan.setOff();         // Fan must remain on while camera is on to prevent damage.
    servo.adjustAngle(0); // Reset servo to neutral position.
    led.ledsOff();
    writer.flush(); // Clear out what is left in buffer.
    writer.closeFile();
    writer.streamFSData(); // Wait for communication from usb FOREVER.
  }

  // Fan is only turned back on after time to reach apogee has been exceeded.
  if (millis() > (fifteenSecs + logStart))
  {
    fan.setOn(); // Fan must remain on while camera is on always to prevent damage.
  }

  // Manual delay to slow down rate of logging. Makes goofy jittery sound because
  // beep is for duration of the delay. This also acts as an audible indicator of state.
  speaker.beep(0.002);
}