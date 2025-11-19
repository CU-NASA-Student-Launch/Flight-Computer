#include <SpeakerControl.hpp>

SpeakerControl::SpeakerControl()
{
    // Setup speaker pins
    pinMode(negPin, OUTPUT);
    pinMode(posPin, OUTPUT);
}

void SpeakerControl::beep(double seconds)
{
    analogWrite(negPin, -128);
    analogWrite(negPin, 128);
    delay(seconds*1000);
    analogWrite(negPin, 0);
    analogWrite(negPin, 0);
}

void SpeakerControl::startAlarm()
{
    analogWrite(negPin, -128);
    analogWrite(negPin, 128);
}

void SpeakerControl::stopAlarm()
{
    analogWrite(negPin, 0);
    analogWrite(negPin, 0);
}