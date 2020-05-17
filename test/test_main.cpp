#include <Arduino.h>
#include <unity.h>

#include "test_triggerevent.hpp"
#include "test_feeder.hpp"

void setup() 
{
    delay(2000);

    UNITY_BEGIN();
    RUN_TRIGGEREVENT_TESTS();
    RUN_FEEDER_TESTS();
    UNITY_END();
}

void loop() 
{
    // Don't like testing with a loop...
}