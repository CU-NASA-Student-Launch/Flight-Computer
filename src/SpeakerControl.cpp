#include <SpeakerControl.hpp>

SpeakerControl::SpeakerControl()
{
    // Setup speaker pins
    pinMode(negPin, OUTPUT);
    pinMode(posPin, OUTPUT);
}

void SpeakerControl::beep(int seconds)
{
    analogWrite(negPin, -128);
    analogWrite(negPin, 128);
    delay(seconds*1000); // Beep for 0.5 seconds
    analogWrite(negPin, 0);
    analogWrite(negPin, 0);
}