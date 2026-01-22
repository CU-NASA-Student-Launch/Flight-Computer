#include <Arduino.h>

#ifndef SPEAKER_CONTROL_HPP
#define SPEAKER_CONTROL_HPP

class SpeakerControl
{
public:
    SpeakerControl();
    void beep(double seconds);
    void blare(void);
    void silence(void);

private:
    // Speaker pins
    const char speakPin = 22;
    
};

#endif // SPEAKER_CONTROL_HPP
