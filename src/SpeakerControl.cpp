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

    //tone(speakPin, 1000, );
    //noTone(speakPin);
}

/////////
void SpeakerControl::beginBeep()
{
    tone(speakPin, 220, 250);
    delay(250);
    tone(speakPin, 440, 250);
    delay(250);
    tone(speakPin, 880, 250);
    delay(250);
}

void SpeakerControl::fileCheckBeep()
{
    tone(speakPin, 880, 500);
    delay(500);
    tone(speakPin, 440, 500);
    delay(500);
    tone(speakPin, 220, 500);
    delay(500);
    noTone(speakPin);
}

////////

void SpeakerControl::blare()
{
    tone(speakPin, 1200, 500);
}

void SpeakerControl::silence()
{
    noTone(speakPin);
}