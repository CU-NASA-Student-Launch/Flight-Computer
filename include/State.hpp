#ifndef STATE_HPP
#define STATE_HPP
#include <Arduino.h>

struct State {
    uint8_t key; // the id of the State
    float msTime; // time in milliseconds
    float altASL; // altitude above sea level in meters
    float vx, vy, vz; // velocity components in m/s
    float v; // magnitude of velocity in m/s
    float vMach; // magntitude of velocity in mach
    float airDensity; // density of air at altASL in kg/m^3
    float az; // acceleration in the z
    bool isFull;
};

#endif // STATE_HPP