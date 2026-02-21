#include "Controller.hpp"

using namespace std;

Controller::Controller(float burnoutMass, float burnoutTime, float projectedArea, int samplingRate) {
    BURNOUT_MASS = burnoutMass;
    BURNOUT_TIME = burnoutTime;
    PROJECTED_AREA = projectedArea;
    deploymentLev = 0.0;
    dragProfile = DragProfile();
    SAMPLE_RATE = samplingRate;
}

void Controller::adjustAirbrakes(State currentState, State lastState) {
    float angle = getAngle(currentState);

    // should physically set the angle of the airbrakes
}

float Controller::getAngle(State currentState) {
    // if pre-burnout
    if (currentState.msTime < BURNOUT_TIME) {
        return 0.0;
    }

    float rocketCd;
    // the coefficient of drag equation used is inaccurate at low velocities
    if(currentState.vz > 20) {
        rocketCd = calculateCd(currentState);
        dragProfile.updateProfile(rocketCd, deploymentLev, currentState.vMach);
    }
    else {
        rocketCd = dragProfile.getCd(deploymentLev, currentState.vMach);
    }

    float prediction = predictApogee(currentState, rocketCd, deploymentLev);

    float angle = seekDepTarget(prediction, currentState, rocketCd);

    return angle;
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

float Controller::predictApogee(State state, float currentCd, float deployment) {
    float rho = state.airDensity;
    float vTemp = state.v;
    float yTemp = state.altASL;
    float dt = 0.01;
    float cdLive = currentCd;

    while (vTemp > 0) {
        if (yTemp != state.altASL) {
            // need to write code to update rhoe with std atm map

            // find cd at new velocity
            cdLive = dragProfile.getCd(deployment, (vTemp/speedOfSound(yTemp)));
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

float Controller::seekDepTarget(float prediction, State currentState, float cd) {
    float targetApogee = 1400 + 932; // in ASL
    float trustGain = 0.5;
    float error = (prediction - targetApogee) * trustGain;

    float nrDep = deploymentLev;
    float nrDelta = 0.05;

    bool cutoffReached = false;

    while(abs(error) > 5 && !cutoffReached) {
        while(error > 0) {
            // Current prediction is larger than target, so proceed to the right on the
            // altitude vs. deployment angle curve
            nrDep = nrDep + nrDelta;

            if(nrDep > 0.5) {
                cutoffReached = true;
                nrDep = 0.5;
                break;
            }

            prediction = predictApogee(currentState, cd, nrDep + nrDelta);
            error = (prediction - targetApogee) * trustGain;
        }
        while(error < 0) {
            // Current prediction is less than target, so proceed to the left on the
            // altitude vs. deployment angle curve
            nrDep = nrDep - nrDelta;

            if(nrDep < 0) {
                cutoffReached = true;
                nrDep = 0;
                break;
            }

            prediction = predictApogee(currentState, cd, nrDep - nrDelta);
            error = (prediction - targetApogee) * trustGain;
        }
    }
     
    float maxChange = 0.3 / SAMPLE_RATE;
    float lowerBound = deploymentLev - maxChange;
    float upperBound = deploymentLev + maxChange;
    nrDep = min(max(nrDep, lowerBound), upperBound);
    
    return nrDep;
}