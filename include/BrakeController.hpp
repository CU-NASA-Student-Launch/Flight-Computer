#include "State.hpp"
#include "DragProfile.hpp"
#include <cmath>
#include <algorithm>

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

class BrakeController {
    public:
        BrakeController(float burnoutMass, float burnoutTime, float projectedArea, int samplingRate);
        void adjustAirbrakes(State currentState, State lastState);
    private:
        float getAngle(State currentState);
        float calculateCd(State currentState);
        float predictApogee(State state, float currentCd, float deployment);
        float speedOfSound(float altitudeASL);
        void rk4Step(float& y, float& v, float dt, float rho, float cd);
        void rk4Helper(float& ky, float& kv, float v, float cd, float rho);
        float seekDepTarget(float prediction, State currentState, float cd);
        float rocketBurnoutMass;
        float motorBurnoutTime;
        float rocketArea;
        float deploymentLev;
        DragProfile dragProfile;
        int pollRate;
};

#endif // CONTROLLER_HPP