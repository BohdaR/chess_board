#include "unity.h"
#include "../include/clock.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {

}

// Test formatTime function
void should_return_formatted_time(void) {
    TEST_ASSERT_EQUAL_STRING("09.5", formatTime(9500).c_str());
    TEST_ASSERT_EQUAL_STRING("09.0", formatTime(9000).c_str());

    TEST_ASSERT_EQUAL_STRING("01:00", formatTime(60000).c_str());
    TEST_ASSERT_EQUAL_STRING("59.9", formatTime(59900).c_str());

    TEST_ASSERT_EQUAL_STRING("01:01", formatTime(61000).c_str());
    TEST_ASSERT_EQUAL_STRING("05:30", formatTime(5.5 * 60 * 1000).c_str());
}

// Test decrementTime function
void should_decrement_players_time(void) {
    whitePlayerTime = 180000;
    blackPlayerTime = whitePlayerTime;

    PositionDynamics.whoseMove = WHITE;
    decrementTime();
    TEST_ASSERT_EQUAL(179900, whitePlayerTime);

    PositionDynamics.whoseMove = BLACK;
    decrementTime();
    TEST_ASSERT_EQUAL(179900, blackPlayerTime);
    PositionDynamics.whoseMove = WHITE;
}

// Test applyIncrement function
void should_increment_players_time(void) {
    whitePlayerTime = 180000;
    blackPlayerTime = whitePlayerTime;
    timeIncrement = 2000;

    PositionDynamics.whoseMove = WHITE;
    applyIncrement();
    TEST_ASSERT_EQUAL(182000, whitePlayerTime);

    PositionDynamics.whoseMove = BLACK;
    applyIncrement();
    TEST_ASSERT_EQUAL(182000, blackPlayerTime);
    PositionDynamics.whoseMove = WHITE;
}

// Test updateClock function
void should_update_clock(void) {
    whitePlayerTime = 180000;
    blackPlayerTime = whitePlayerTime;

    PositionDynamics.whoseMove = WHITE;
    updateClock();
    TEST_ASSERT_EQUAL(179900, whitePlayerTime);

    PositionDynamics.whoseMove = BLACK;
    updateClock();
    TEST_ASSERT_EQUAL(179900, blackPlayerTime);
    PositionDynamics.whoseMove = WHITE;
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(should_return_formatted_time);
    RUN_TEST(should_decrement_players_time);
    RUN_TEST(should_increment_players_time);
    RUN_TEST(should_update_clock);
    return UNITY_END();
}


void setup() {
    runUnityTests();
}

void loop() {

}