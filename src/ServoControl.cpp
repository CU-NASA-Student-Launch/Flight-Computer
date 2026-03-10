#include <ServoControl.hpp>

ServoControl::ServoControl()
{

}

void ServoControl::connectServo(void) 
{
    servo.attach(servoOnePin);
}

void ServoControl::adjustAngle(int angle)
{
    servo.write(angle);
}