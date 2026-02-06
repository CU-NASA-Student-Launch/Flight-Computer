#include <DragProfile.hpp>


DragProfile::DragProfile() {
    // Want two vectors (depVals and machVals) that contain
    // all the unique values for each of those axes
    
    for (int i = 0; i < size(dragProfile) - 1; ++i) {
        depVals.push_back(dragProfile[i][0]);
        ++i;
    }
    sort(depVals.begin(),depVals.end());

    // removing duplicates
    depVals.erase(unique(depVals.begin(), depVals.end()), depVals.end());

    for (int i = 0; i < size(dragProfile) - 1; ++i) {
        machVals.push_back(dragProfile[i][1]);
        ++i;
    }
    sort(machVals.begin(),machVals.end());

    // removing duplicates
    machVals.erase(unique(machVals.begin(), machVals.end()), machVals.end());
}

float DragProfile::getDrag(float deployment, float mach) {
    // clamping inputs
    if (deployment < 0) deployment = 0;
    if (deployment > 1) deployment = 1;
    if (mach < 0) mach = 0;
    if (mach > 1) mach = 0.5;


    // finding indices of bounding values for deployment levels
    int d0 = 0;
    int d1 = 0;
    for (int i = 0; i < size(depVals) - 1; ++i) {
        if (deployment >= depVals[i] && deployment <= depVals[i+1]) {
            d0 = i;
            d1 = i + 1;
        }
    }

    // finding indices of bounding values for mach levels
    int m0 = 0;
    int m1 = 0;
    for (int i = 0; i < size(machVals) - 1; ++i) {
        if (mach >= machVals[i] && mach <= machVals[i+1]) {
            m0 = i;
            m1 = i + 1;
        }
    }

    // Calculate fractional distance away from smaller bounding value
    float depFrac = (deployment - depVals[d0]) / (depVals[d1] - depVals[d0]);
    float machFrac = (mach - machVals[m0]) / (machVals[m1] - machVals[m0]);
    
    // Find values in Cd values in drag matrix
    int columns = size(machVals);
    float lowDlowM = dragProfile[d0 * columns + m0][2];
    float lowDhighM = dragProfile[d0 * columns + m1][2];
    float highDlowM = dragProfile[d1 * columns + m0][2];
    float highDhighM = dragProfile[d1 * columns + m1][2];
    
    // Interpolate
    float lowD = lowDlowM + (lowDhighM - lowDlowM) * machFrac;
    float highD = highDlowM + (highDhighM - highDlowM) * machFrac;
    return lowD + (highD - lowD) * depFrac;
}