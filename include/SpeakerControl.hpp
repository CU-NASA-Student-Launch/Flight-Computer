#include <Arduino.h>

#ifndef SPEAKER_CONTROL_HPP
#define SPEAKER_CONTROL_HPP

class SpeakerControl
{
public:
    SpeakerControl();
    void beep(double seconds);
    void startAlarm(void);
    void stopAlarm(void);

private:
    // Speaker pins
    const char posPin = 22;
    const char negPin = 24;
    
};

#endif // SPEAKER_CONTROL_HPP
