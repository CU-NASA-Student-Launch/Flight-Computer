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
    analogWrite(posPin, 128);
    delay(seconds*1000);
    analogWrite(negPin, 0);
    analogWrite(posPin, 0);
}

void SpeakerControl::blare()
{
    analogWrite(negPin, -128);
    analogWrite(posPin, 128);
    delay(1000);
}