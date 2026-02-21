#include "State.hpp"

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

using namespace std;

class Controller {
    public:
        Controller();
        void AdjustAirbrakes(State currentState, State lastState);
    private:
};

#endif // CONTROLLER_HPP