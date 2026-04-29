#include <LEDControl.hpp>

LEDControl::LEDControl()
{
    pinMode(ledPinOne, OUTPUT);
    pinMode(ledPinTwo, OUTPUT);
}

void LEDControl::leftLedOn(void)
{
    analogWrite(ledPinOne, 0);
    analogWrite(ledPinTwo, 128);
}

void LEDControl::rightLedOn(void)
{
    analogWrite(ledPinOne, 128);
    analogWrite(ledPinTwo, 0);
}

void LEDControl::ledsOn(void)
{
    analogWrite(ledPinOne, 128);
    analogWrite(ledPinTwo, 128);
}

void LEDControl::ledsOff(void)
{
    analogWrite(ledPinOne, 0);
    analogWrite(ledPinTwo, 0);
}