#include <FanControl.hpp>

FanControl::FanControl()
{
    // Setup speaker pins
    pinMode(fanPin, OUTPUT);
}

void FanControl::setOn(void)
{
    digitalWrite(fanPin, HIGH);
}

void FanControl::setOff(void)
{
    digitalWrite(fanPin, LOW);
}

