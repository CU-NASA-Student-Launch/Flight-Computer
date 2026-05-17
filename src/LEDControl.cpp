#include <LEDControl.hpp>

LEDControl::LEDControl()
{
    pinMode(ledPinOne, OUTPUT);
    pinMode(ledPinTwo, OUTPUT);
}

void LEDControl::leftLedOn(void)
{
    digitalWrite(ledPinOne, LOW);
    digitalWrite(ledPinTwo, HIGH);
}

void LEDControl::rightLedOn(void)
{
    digitalWrite(ledPinOne, HIGH);
    digitalWrite(ledPinTwo, LOW);
}

void LEDControl::ledsOn(void)
{
    digitalWrite(ledPinOne, HIGH);
    digitalWrite(ledPinTwo, HIGH);
}

void LEDControl::ledsOff(void)
{
    digitalWrite(ledPinOne, LOW);
    digitalWrite(ledPinTwo, LOW);
}