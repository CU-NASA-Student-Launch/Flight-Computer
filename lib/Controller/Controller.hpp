#include "State.hpp"
#include "DragProfile.hpp"
#include <cmath>

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

using namespace std;

class Controller {
    public:
        Controller(float burnoutMass, float burnoutTime, float projectedArea);
        void adjustAirbrakes(State currentState, State lastState);
    private:
        float getAngle(State currentState);
        float calculateCd(State currentState);
        float predictApogee(State currentState, float currentCd);
        float speedOfSound(float altitudeASL);
        void rk4Step(float& y, float& v, float dt, float rho, float cd);
        float rk4Helper(float& ky, float& kv, float v, float cd, float rho);
        float BURNOUT_MASS;
        float BURNOUT_TIME;
        float PROJECTED_AREA;
        float deploymentLev;
        DragProfile dragProfile;
};

#endif // CONTROLLER_HPP