#include <SpeakerControl.hpp>

SpeakerControl::SpeakerControl()
{
    // Setup speaker pins
    pinMode(speakPin, OUTPUT);
}

void SpeakerControl::beep(double seconds)
{
    analogWrite(speakPin, 128);
    delay(seconds*1000);
    analogWrite(speakPin, 0);
}

void SpeakerControl::blare()
{
    analogWrite(speakPin, 128);
    delay(1000);
}

void SpeakerControl::silence()
{
    analogWrite(speakPin, 0);
}