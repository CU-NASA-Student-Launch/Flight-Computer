#ifndef STATE_HPP
#define STATE_HPP

#endif // STATE_HPP

using namespace std;

struct State {
    unsigned long msTime; // time in milliseconds
    float altASL; // altitude above sea level in meters
    float vx, vy, vz; // velocity components in m/s
    float vMach; // magntitude of velocity in mach
    float airDensity; // density of air at altASL in kg/m^3
    float az; // acceleration in the z
    
};