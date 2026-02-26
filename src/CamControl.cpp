#include <CamControl.hpp>

CamControl::CamControl()
{
    // Setup speaker pins
    pinMode(camPin, OUTPUT);
}

void CamControl::setOn(void)
{
    analogWrite(camPin, 128);
}

void CamControl::setOff(void)
{
    analogWrite(camPin, 0);
}

