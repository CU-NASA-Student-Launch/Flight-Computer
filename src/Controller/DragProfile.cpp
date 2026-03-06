#include "DragProfile.hpp"

DragProfile::DragProfile() {
    // Want two vectors (depVals and machVals) that contain
    // all the unique values for each of those axes
    
    for (int i = 0; i < 35; ++i) {
        depVals.push_back(dragProfile[i][0]);
    }
    std::sort(depVals.begin(),depVals.end());

    // removing duplicates
    depVals.erase(std::unique(depVals.begin(), depVals.end()), depVals.end());

    for (int i = 0; i < 35; ++i) {
        machVals.push_back(dragProfile[i][1]);
    }
    std::sort(machVals.begin(),machVals.end());

    // removing duplicates
    machVals.erase(std::unique(machVals.begin(), machVals.end()), machVals.end());
}

/**
 * @brief Gets the drag coefficient given a normalized deployment level and a mach number.
 * @note The deployment is clamped between 0 and 1, and the mach is clamped between 0 and 0.5 respectively.
 * 
 * @param deployment The deployment level normalized between 0 and 1.
 * @param mach The speed of the rocket in mach.
 * @return The coefficient of drag.
 */
float DragProfile::getCd(float deployment, float mach) {
    // clamping inputs
    if (deployment < 0) deployment = 0;
    if (deployment > 1) deployment = 1;
    if (mach < 0) mach = 0;
    if (mach > 0.5) mach = 0.5;


    // finding indices of bounding values for deployment levels
    int d0 = 0;
    int d1 = 0;
    for (int i = 0; i < depVals.size() - 1; ++i) {
        if (deployment >= depVals[i] && deployment <= depVals[i+1]) {
            d0 = i;
            d1 = i + 1;
        }
    }

    // finding indices of bounding values for mach levels
    int m0 = 0;
    int m1 = 0;
    for (int i = 0; i < machVals.size() - 1; ++i) {
        if (mach >= machVals[i] && mach <= machVals[i+1]) {
            m0 = i;
            m1 = i + 1;
        }
    }

    // Calculate fractional distance away from smaller bounding value
    float depFrac = (deployment - depVals[d0]) / (depVals[d1] - depVals[d0]);
    float machFrac = (mach - machVals[m0]) / (machVals[m1] - machVals[m0]);
    
    // Find values in Cd values in drag matrix
    int columns = machVals.size();
    float lowDlowM = dragProfile[d0 * columns + m0][2];
    float lowDhighM = dragProfile[d0 * columns + m1][2];
    float highDlowM = dragProfile[d1 * columns + m0][2];
    float highDhighM = dragProfile[d1 * columns + m1][2];
    
    // Interpolate
    float lowD = lowDlowM + (lowDhighM - lowDlowM) * machFrac;
    float highD = highDlowM + (highDhighM - highDlowM) * machFrac;
    return lowD + (highD - lowD) * depFrac;
}

/**
 * @brief Updates the coefficient of drag profile by offseting the entire curve by Cd error/difference.
 * @note The deployment is clamped between 0 and 1, and the mach is clamped between 0 and 0.5 respectively.
 * 
 * @param measuredCd The coefficient of drag measured at the current time.
 * @param deployment The deployment level normalized between 0 and 1.
 * @param mach The speed of the rocket in mach.
 * @return The difference between the measuredCd and the profile Cd at the given deployment and mach number.
 */
float DragProfile::updateProfile(float measuredCd, float deployment, float mach) {
    // clamping occurs inside of the getDrag function
    float profileCd = getCd(deployment, mach);
    float offset = measuredCd - profileCd;

    for(int i = 0; i < 35; ++i) {
        dragProfile[i][2] = dragProfile[i][2] + offset;
    }

    return offset;
}