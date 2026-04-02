#include "BrakeController.hpp"

BrakeController::BrakeController(float burnoutMass, float burnoutTime, float projectedArea, int samplingRate) {
    rocketBurnoutMass = burnoutMass;
    motorBurnoutTime = burnoutTime;
    rocketArea = projectedArea;
    deploymentLev = 0.0;
    dragProfile = DragProfile();
    pollRate = samplingRate;

    FLAP_POSITIONS = linspace(0.0, 1.0, 401);
    INITIAL_MAGIC_ANGLE = 0.6;

    logger.initiate();
}

void BrakeController::adjustAirbrakes(State currentState, State lastState) {
    float angle = getAngle(currentState);

    // should physically set the angle of the airbrakes
}

float BrakeController::getAngle(State currentState) {
    unsigned long startTime = millis();
    unsigned long endTime;
    
    int targetIndex;
    float rocketCd;
    
    // if pre-burnout
    // need to change this to detect negative acceleration for burnout detection
    if (currentState.msTime < motorBurnoutTime) {
        targetIndex = 0;
    }
    // 2 seconds to get to magic angle
    else if (currentState.msTime <= motorBurnoutTime + 2000) {
        float rampFraction = (currentState.msTime - motorBurnoutTime) / 2000;
        float rampContinuous = INITIAL_MAGIC_ANGLE * rampFraction;

        targetIndex = findClosestIndex(FLAP_POSITIONS, rampContinuous);
        deploymentLev = FLAP_POSITIONS[targetIndex];
        
        logEntry.angle = deploymentLev;
        logEntry.flapIndex = targetIndex;
        logEntry.airDensity = currentState.airDensity;
        logEntry.altASL = currentState.altASL;
        logEntry.az = currentState.az;
        logEntry.key = currentState.key;
        logEntry.msTime = currentState.msTime;
        logEntry.v = currentState.v;
        logEntry.vMach = currentState.vMach;
        logEntry.vx = currentState.vx;
        logEntry.vy = currentState.vy;
        logEntry.vz = currentState.vz;
        logEntry.cd = NAN;
        logEntry.undershoot = false;
        logEntry.overshoot = false;
        logEntry.stage = 0;

        logEntry.timeOfCompute = millis() - startTime;

        logger.store(logEntry);

        return deploymentLev;
    }    
    
    else if(currentState.msTime < motorBurnoutTime + 5000) {
        targetIndex = findClosestIndex(FLAP_POSITIONS,INITIAL_MAGIC_ANGLE);

        // deploymentLev = FLAP_POSITIONS[targetIndex];
        deploymentLev = INITIAL_MAGIC_ANGLE;

        logEntry.angle = deploymentLev;
        logEntry.flapIndex = targetIndex;
        logEntry.airDensity = currentState.airDensity;
        logEntry.altASL = currentState.altASL;
        logEntry.az = currentState.az;
        logEntry.key = currentState.key;
        logEntry.msTime = currentState.msTime;
        logEntry.v = currentState.v;
        logEntry.vMach = currentState.vMach;
        logEntry.vx = currentState.vx;
        logEntry.vy = currentState.vy;
        logEntry.vz = currentState.vz;
        logEntry.cd = NAN;
        logEntry.undershoot = false;
        logEntry.overshoot = false;
        logEntry.stage = 1;

        logEntry.timeOfCompute = millis() - startTime;

        logger.store(logEntry);

        return deploymentLev;
    }

    else {

        // the coefficient of drag equation used is inaccurate at low velocities
        if(currentState.vz > 40) {
            rocketCd = calculateCd(currentState);
            dragProfile.updateProfile(rocketCd, deploymentLev, currentState.vMach);
        }
        else {
            rocketCd = dragProfile.getCd(deploymentLev, currentState.vMach);
        }

    }

    float prediction = predictApogee(currentState, rocketCd, deploymentLev);

    float angle = seekDepTarget(prediction, currentState, rocketCd, startTime);

    return angle;
}

float BrakeController::calculateCd(State currentState) {
    float numer = rocketBurnoutMass * abs(-9.807 - currentState.az);
    float denom = 0.5 * currentState.airDensity * currentState.vz * currentState.vz 
        * (rocketArea + 0.0001 * (deploymentLev * 40));
    
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

float BrakeController::predictApogee(State state, float currentCd, float deployment) {
    float rho = state.airDensity;
    float vTemp = state.v;
    float yTemp = state.altASL;
    float dt = 0.01;
    float cdLive = currentCd;
    float area = 0.0092;

    while (vTemp > 0) {
        if (yTemp != state.altASL) {
            // need to write code to update rho with std atm map

            // find cd at new velocity
            cdLive = dragProfile.getCd(deployment, (vTemp/speedOfSound(yTemp)));

            // dynamic area update
            area = (-5*pow(10, -7)) * pow((deployment * 40), 2) + 0.0001 * (deployment * 40) + 0.0092;
        }

        rk4Step(yTemp, vTemp, dt, rho, cdLive, area);
    }

    return yTemp;
}

float BrakeController::speedOfSound(float altitudeASL) {
    return 360; // could change this to dynamic if needed
}

void BrakeController::rk4Step(float& y, float& v, float dt, float rho, float cd, float area) {
    float k1y, k1v;
    float k2y, k2v;
    float k3y, k3v;
    float k4y, k4v;

    rk4Helper(k1y, k1v, v, cd, rho, area);
    rk4Helper(k2y, k2v, v + 0.5 * dt * k1v, cd, rho, area);
    rk4Helper(k3y, k3v, v + 0.5 * dt * k2v, cd, rho, area);
    rk4Helper(k4y, k4v, v + dt * k3v, cd, rho, area);

    y = y + (dt/6.0) * (k1y + 2 * k2y + 2 * k3y + k4y);
    v = v + (dt / 6.0) * (k1v + 2 * k2v + 2 * k3v + k4v);
}

void BrakeController::rk4Helper(float& ky, float& kv, float v, float cd, float rho, float area) {
    float drag = (0.5 * rho * cd * area * v * abs(v))/rocketBurnoutMass;
    ky = v;
    kv = -9.807 - drag;
}

float BrakeController::seekDepTarget(float prediction, State currentState, float cd, unsigned long startTime) {
    float targetApogee = 1400 + 324; // in ASL
    float error = prediction - targetApogee;
    float targetDeployment;
    bool targetLocked = false;

    // Evaluating bounds 0 and 1
    float errorLo = predictApogee(currentState, cd, 0) - targetApogee;
    float errorHi = predictApogee(currentState, cd, 1) - targetApogee;

    logEntry.undershoot = false;
    logEntry.overshoot = false;

    // --- Case 1: undershoot even with no drag ---
    if (errorLo < 0) {
        targetDeployment = 0.0;
        targetLocked = true;
        logEntry.undershoot = true;
    }   

    // --- Case 2: overshoot even with full drag ---
    if (errorHi > 0) {
        if(!targetLocked) { targetDeployment = 1.0; }
        targetLocked = true;
        logEntry.overshoot = true;
    }
    

    float lo = 0.0;
    float hi = 1.0;
    float mid;
    float midError;

    // --- Case 3: solution exists inside [0,1] ---
    for(int i = 0; i < 25; ++i) {
        if(targetLocked) {
            break;
        }

        mid = 0.5 * (lo + hi);
        midError = predictApogee(currentState, cd, mid) - targetApogee;

        if (abs(midError) < 14.0) {
            targetDeployment = mid;
            targetLocked = true;
            break;
        }

        if (midError > 0) {
            lo = mid;
        }
        else {
            hi = mid;
        }
    }
    
    if(!targetLocked) { targetDeployment = 0.5 * (lo + hi); }
    
    size_t targetIndex = findClosestIndex(FLAP_POSITIONS, targetDeployment);

    deploymentLev = FLAP_POSITIONS[targetIndex];

    logEntry.angle = deploymentLev;
    logEntry.flapIndex = targetIndex;
    logEntry.airDensity = currentState.airDensity;
    logEntry.altASL = currentState.altASL;
    logEntry.az = currentState.az;
    logEntry.key = currentState.key;
    logEntry.msTime = currentState.msTime;
    logEntry.v = currentState.v;
    logEntry.vMach = currentState.vMach;
    logEntry.vx = currentState.vx;
    logEntry.vy = currentState.vy;
    logEntry.vz = currentState.vz;
    logEntry.cd = cd;
    logEntry.stage = 2;

    logEntry.timeOfCompute = millis() - startTime;

    logger.store(logEntry);

    return deploymentLev;
}

// Helper functions
std::vector<double> BrakeController::linspace(double start, double stop, std::size_t num) {
    std::vector<double> result;
    result.reserve(num);

    if(num == 0) return result;
    if (num == 1) {
        result.push_back(start);
        return result;
    }

    double step = (stop - start) / (num - 1);

    for (std::size_t i = 0; i < num; ++i) {
        result.push_back(start + i * step);
    }

    return result;
}

size_t BrakeController::findClosestIndex(const std::vector<double>& vec, float target) {
    // if (vec.empty()) {
    //     throw std::runtime_error("Vector is empty");
    // }

    auto it = std::lower_bound(vec.begin(), vec.end(), target);

    // If target is smaller than all elements
    if (it == vec.begin()) {
        return 0;
    }

    // If target is larger than all elements
    if (it == vec.end()) {
        return vec.size() - 1;
    }

    // Compare element before and after lower_bound
    size_t idx_after = std::distance(vec.begin(), it);
    size_t idx_before = idx_after - 1;

    float after = vec[idx_after];
    float before = vec[idx_before];

    if (std::fabs(target - before) <= std::fabs(target - after)) {
        return idx_before;
    } else {
        return idx_after;
    }
}



// Kinematics helper functions for airbrakes — these are just the curve-fit polynomials
float Fd_equation(float v, float theta) {
    return (
        0.23817 * v 
        - 0.0024227 * powf(v, 2)
        + 0.000040072 * powf(v, 3)
        - 0.00000011152 * powf(v, 4)
        + 0.0035825 * theta * v
        - 0.000024523 * theta * powf(v, 2)
        + 0.00000059048 * theta * powf(v, 3)
        - 0.0000000016792 * theta * powf(v, 4)
    );
}


// Air density as a function of velocity and flap angle (via Fd).
float rho_equation(float v, float theta) {
    return (
        1.0508
        + 0.0098085 * Fd_equation(v, theta)
        - 0.0026155 * v
        - 0.00020012 * Fd_equation(v, theta) * v
        + 0.00005843 * powf(v, 2)
        + 0.000001422 * Fd_equation(v, theta) * powf(v, 2)
        - 0.00000039555 * powf(v, 3)
        - 0.0000000034329 * Fd_equation(v, theta) * powf(v, 3)
        + 0.00000000097234 * powf(v, 4)
    ); 
};


// Projected area as a function of flap angle, "FA".
float projectedArea(float FA) {
    return (
        0.0000000006313333 * powf(FA, 4)
        - 0.00000007840309 * powf(FA, 3)
        + 0.000002740515 * powf(FA, 2)
        + 0.0000722568 * FA
        + 0.009551555
    );
};


//CD surface as a function of drag, "FD_equation"; velocity, "v"; density, "rho_equation"; and the projected area, "projectedArea".
float CD_equation(float Fd_equation, float v, float rho_equation, float projectedArea) {
    return (
        Fd_equation/(0.5 * rho_equation * powf(v, 2) * projectedArea)
    );
}
