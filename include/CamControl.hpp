#include <Arduino.h>

#ifndef CAM_CONTROL_HPP
#define CAM_CONTROL_HPP

class CamControl
{
public:
    CamControl();
    void setOn(void);
    void setOff(void);

private:
    // Speaker pins
    const char camPin = 18;
    
};

#endif // CAM_CONTROL_HPP
