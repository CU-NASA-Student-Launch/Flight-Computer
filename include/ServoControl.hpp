#include <Arduino.h>
#include <Servo.h>

#ifndef SERVO_CONTROL_HPP
#define SERVO_CONTROL_HPP

class ServoControl
{
public:
    ServoControl();
    void connectServo();
    void adjustAngle(int degrees);

private:
    Servo servo;
    const char servoOnePin = 17;
    const char servoTwoPin = 16;
};

#endif // SERVO_CONTROL_HPP
