#pragma once

#include <unity.h>
#include "appconfig.h"
#include "feeder.h"
#include "triggerevent.h"

#include "test_util.hpp"

AF_Stepper _testStepper(STEPPER_STEPS, STEPPER_PORT);

void should_feed_when_event_triggered(void) 
{
    Feeder fe(_testStepper);
    TriggerEvent<mock_millis> te(TRIGGER_EVENT_TIME, [](void* p){
        const auto feeder = (Feeder*)p;
        feeder->Feed();
    }, (void*)&fe);

    auto oldCount = fe.GetFeedCount();
    mock_millis_elapse(TRIGGER_EVENT_TIME);
    te.Update();

    auto newCount = fe.GetFeedCount();
    TEST_ASSERT_TRUE(newCount == (oldCount + 1));
}

void RUN_FEEDER_TESTS()
{
    _testStepper.setSpeed(STEPPER_RPM);

    RUN_TEST(should_feed_when_event_triggered);
}