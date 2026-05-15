#include <ServoControl.hpp>

ServoControl::ServoControl()
{
}

void ServoControl::connectServo(void)
{
    servo.attach(servoOnePin);
}

// To keep things more intuative, this function takes in angles beween -60 and 60 degrees
// off of neutral and adjusts them to be between 0-180.
// It also doesn't allow angles that would hit the hard stop.
void ServoControl::adjustAngle(float degrees)
{
    int adjustedAngle = static_cast<int>((degrees + 40.145) / 0.4436);
    // These two angles are safe assuming canard starts virtical
    int maxAngle = 180;
    int minAngle = 0;

    if (adjustedAngle > maxAngle)
    {
        servo.write(maxAngle);
    }
    else if (adjustedAngle < minAngle)
    {
        servo.write(minAngle);
    }
    else
    {
        servo.write(adjustedAngle);
    }
}