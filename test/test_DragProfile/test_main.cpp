#include <unity.h>
#include "DragProfile.hpp"

void test_getDrag_function() {
    DragProfile dragProfile = DragProfile();

    // testing point in the middle of the profile
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.629184187f, dragProfile.getCd(0.75, 0.30));
    
    // testing edges
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.0f, dragProfile.getCd(0.00, 0.00));
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.705007922f, dragProfile.getCd(1.0, 0.5));

    // testing clamping
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.0f, dragProfile.getCd(-1.00, -1.00));
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.705007922f, dragProfile.getCd(1.5, 3.0));
}

void test_updateDrag_function() {
    DragProfile dragProfile = DragProfile();

    // testing point in the middle of the profile
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.629184187f, dragProfile.getCd(0.75, 0.30));
    
    // testing edges
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.0f, dragProfile.getCd(0.00, 0.00));
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.705007922f, dragProfile.getCd(1.0, 0.5));

    // testing clamping
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.0f, dragProfile.getCd(-1.00, -1.00));
    TEST_ASSERT_FLOAT_WITHIN(0.00000001f, 0.705007922f, dragProfile.getCd(1.5, 3.0));

    // offset of 0.2
    float offset = dragProfile.updateProfile(0.829184187, 0.75, 0.30); // measuredCd value was selected with an offset of 0.2

    TEST_ASSERT_FLOAT_WITHIN(0.000001f, 0.2f, offset);

    // testing point in the middle of the profile
    TEST_ASSERT_FLOAT_WITHIN(0.000001f, 0.629184187f + offset, dragProfile.getCd(0.75, 0.30));
    
    // testing edges
    TEST_ASSERT_FLOAT_WITHIN(0.000001f, 0.0f + offset, dragProfile.getCd(0.00, 0.00));
    TEST_ASSERT_FLOAT_WITHIN(0.000001f, 0.705007922f + offset, dragProfile.getCd(1.0, 0.5));

    // testing clamping
    TEST_ASSERT_FLOAT_WITHIN(0.000001f, 0.0f + offset, dragProfile.getCd(-1.00, -1.00));
    TEST_ASSERT_FLOAT_WITHIN(0.000001f, 0.705007922f + offset, dragProfile.getCd(1.5, 3.0));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_getDrag_function);
    RUN_TEST(test_updateDrag_function);
    return UNITY_END();
}
