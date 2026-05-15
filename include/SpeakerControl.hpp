#include <Arduino.h>

#ifndef SPEAKER_CONTROL_HPP
#define SPEAKER_CONTROL_HPP

class SpeakerControl
{
public:
    SpeakerControl();
    void beep(unsigned int frequency);
    void blare(void);
    void silence(void);
    void beginBeep(void);
    void fileCheckBeep(void);

private:
    // Speaker pins
    const char speakPin = 22;
};

#endif // SPEAKER_CONTROL_HPP
