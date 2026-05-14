#include <CamControl.hpp>

CamControl::CamControl()
{
    // Setup speaker pins
    pinMode(camPin, OUTPUT);
}

void CamControl::setOn(void)
{
    digitalWrite(camPin, HIGH);
}

void CamControl::setOff(void)
{
    digitalWrite(camPin, LOW);
}

