#include "Controller.hpp"

using namespace std;

Controller::Controller(float burnoutMass, float burnoutTime, float projectedArea) {
    BURNOUT_MASS = burnoutMass;
    BURNOUT_TIME = burnoutTime;
    PROJECTED_AREA = projectedArea;
    deploymentLev = 0.0;
    dragProfile = DragProfile();
}

void Controller::adjustAirbrakes(State currentState, State lastState) {
    // should physically set the angle of the airbrakes
}

float Controller::getAngle(State currentState) {
    // if pre-burnout
    if (currentState.msTime < BURNOUT_TIME) {
        return 0.0;
    }

    // the coefficient of drag equation used is inaccurate at low velocities
    if(currentState.vz > 20) {
        float rocketCd = calculateCd(currentState);
        dragProfile.updateProfile(rocketCd, deploymentLev, currentState.vMach);
    }
    else {
        float rocketCd = dragProfile.getCd(deploymentLev, currentState.vMach);
    }
}

float Controller::calculateCd(State currentState) {
    float numer = BURNOUT_MASS * abs(-9.807 - currentState.az);
    float denom = 0.5 * currentState.airDensity * currentState.vz * currentState.vz 
        * (PROJECTED_AREA + 0.0001 * (deploymentLev * 40));
    
    float cd = numer/denom;

    // clipping
    if(cd > 1) {
        cd = 1;
    }
    else if(cd < 0) {
        cd = 0;
    }

    return cd;
}

float Controller::predictApogee(State currentState, float currentCd) {
    float rho = currentState.airDensity;
    float vTemp = currentState.v;
    float yTemp = currentState.altASL;
    float dt = 0.01;
    float cdLive = currentCd;

    while (vTemp > 0) {
        if (yTemp != currentState.altASL) {
            // need to write code to update rhoe with std atm map

            // find cd at new velocity
            cdLive = dragProfile.getCd(deploymentLev, (vTemp/speedOfSound(yTemp)));
        }

        rk4Step(yTemp, vTemp, dt, rho, cdLive);
    }

    return yTemp;
}

float Controller::speedOfSound(float altitudeASL) {

}

void Controller::rk4Step(float& y, float& v, float dt, float rho, float cd) {
    float k1y, k1v;
    float k2y, k2v;
    float k3y, k3v;
    float k4y, k4v;

    rk4Helper(k1y, k1v, v, cd, rho);
    rk4Helper(k2y, k2v, v + 0.5 * dt * k1v, cd, rho);
    rk4Helper(k3y, k3v, v + 0.5 * dt * k2v, cd, rho);
    rk4Helper(k4y, k4v, v + dt * k3v, cd, rho);

    y = y + (dt/6.0) * (k1y + 2 * k2y + 2 * k3y + k4y);
    v = v + (dt / 6.0) * (k1v + 2 * k2v + 2 * k3v + k4v);
}

float Controller::rk4Helper(float& ky, float& kv, float v, float cd, float rho) {
    float refArea = 0.001;
    ky = v;
    kv = -9.807 - 0.5 * rho * cd * 0.001/BURNOUT_MASS * v * abs(v);
}