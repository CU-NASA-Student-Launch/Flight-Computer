#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "State.hpp"
#include "DragProfile.hpp"
// #include "Logging/LogEntry.hpp"
// #include "Logging/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

class BrakeController {
    public:
        BrakeController(float burnoutMass, float burnoutTime, float projectedArea, int samplingRate);
        void adjustAirbrakes(State currentState, State lastState);
        float getAngle(State currentState);
    private:
        float calculateCd(State currentState);
        float predictApogee(State state, float currentCd, float deployment);
        float speedOfSound(float altitudeASL);
        void rk4Step(float& y, float& v, float dt, float rho, float cd, float area);
        void rk4Helper(float& ky, float& kv, float v, float cd, float rho, float area);
        float seekDepTarget(float prediction, State currentState, float cd, unsigned long startTime);
        float rocketBurnoutMass;
        float motorBurnoutTime;
        float rocketArea;
        float deploymentLev;
        DragProfile dragProfile;
        int pollRate;
        Logger logger;
        LogEntry logEntry;
        std::vector<double> FLAP_POSITIONS;
        float INITIAL_MAGIC_ANGLE;

        // some helper functions
        static std::vector<double> linspace(double start, double stop, std::size_t num);
        static size_t findClosestIndex(const std::vector<double>& vec, float target);
};

#endif // CONTROLLER_HPP