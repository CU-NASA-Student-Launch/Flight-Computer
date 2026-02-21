#include "State.hpp"
#include "DragProfile.hpp"
#include <cmath>
#include <algorithm>

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

using namespace std;

class Controller {
    public:
        Controller(float burnoutMass, float burnoutTime, float projectedArea, int samplingRate);
        void adjustAirbrakes(State currentState, State lastState);
    private:
        float getAngle(State currentState);
        float calculateCd(State currentState);
        float predictApogee(State state, float currentCd, float deployment);
        float speedOfSound(float altitudeASL);
        void rk4Step(float& y, float& v, float dt, float rho, float cd);
        float rk4Helper(float& ky, float& kv, float v, float cd, float rho);
        float seekDepTarget(float prediction, State currentState, float cd);
        float BURNOUT_MASS;
        float BURNOUT_TIME;
        float PROJECTED_AREA;
        float deploymentLev;
        DragProfile dragProfile;
        int SAMPLE_RATE;
};

#endif // CONTROLLER_HPP