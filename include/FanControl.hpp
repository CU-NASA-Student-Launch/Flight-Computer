#include <Arduino.h>

#ifndef FAN_CONTROL_HPP
#define FAN_CONTROL_HPP

class FanControl
{
public:
    FanControl();
    void setOn(void);
    void setOff(void);

private:
    // Speaker pins
    const char fanPin = 19;
    
};

#endif // SPEAKER_CONTROL_HPP
