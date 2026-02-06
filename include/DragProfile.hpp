#ifndef DRAG_PROFILE_HPP
#define DRAG_PROFILE_HPP

#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

class DragProfile {
    public:
        DragProfile();
        float getDrag(float deployment, float mach);
    
    private:
        // Want two vectors (depVals and machVals) that contain
        // all the unique values for each of those axes
    
        vector<float> depVals;
        vector<float> machVals;

        float dragProfile[32][3] = {
            {1, 0, 0},   
            {1, 0.10, 0.705007922},
            {1, 0.15, 0.705007922},
            {1, 0.20, 0.705007922},
            {1, 0.30, 0.705007922},
            {1, 0.40, 0.705007922},
            {1, 0.50, 0.705007922},

            {0.75, 0.10, 0.629184187},
            {0.75, 0.15, 0.629184187},
            {0.75, 0.20, 0.629184187},
            {0.75, 0.30, 0.629184187},
            {0.75, 0.40, 0.629184187},
            {0.75, 0.50, 0.629184187},

            {0.50, 0.10, 0.551564674},
            {0.50, 0.15, 0.551564674},
            {0.50, 0.20, 0.551564674},
            {0.50, 0.30, 0.551564674},
            {0.50, 0.40, 0.551564674},
            {0.50, 0.50, 0.551564674},

            {0.25, 0.10, 0.459472308},
            {0.25, 0.15, 0.459472308},
            {0.25, 0.20, 0.459472308},
            {0.25, 0.30, 0.459472308},
            {0.25, 0.40, 0.459472308},
            {0.25, 0.50, 0.459472308},
            
            {0, 0.10, 0.455905238},
            {0, 0.15, 0.455905238},
            {0, 0.20, 0.455905238},
            {0, 0.30, 0.455905238},
            {0, 0.40, 0.455905238},
            {0, 0.50, 0.455905238},
            {0, 0, 0}
        };
};

#endif // DRAG_PROFILE_HPP