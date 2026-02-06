#include <FanControl.hpp>

FanControl::FanControl()
{
    // Setup speaker pins
    pinMode(fanPin, OUTPUT);
}

void FanControl::setOn(void)
{
    analogWrite(fanPin, 128);
}

void FanControl::setOff(void)
{
    analogWrite(fanPin, 0);
}

