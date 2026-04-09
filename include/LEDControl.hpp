#include <Arduino.h>

#ifndef LED_CONTROL_HPP
#define LED_CONTROL_HPP

class LEDControl
{
public:
    LEDControl();
    void leftLedOn(void);
    void rightLedOn(void);
    void ledsOff(void);

private:
    // LED pins
    const char ledPinOne = 19; // Right LED
    const char ledPinTwo = 20; // Left LED
};

#endif // SPEAKER_CONTROL_HPP
