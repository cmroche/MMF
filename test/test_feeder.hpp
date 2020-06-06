#pragma once

#include <unity.h>
#include "appconfig.h"
#include "feeder.h"
#include "timetriggerevent.h"

#include "test_util.hpp"

//AF_Stepper _testStepper(STEPPER_STEPS, STEPPER_PORT);

void should_feed_when_event_triggered(void) 
{
    Feeder fe;
    auto oldCount = fe.GetFeedCount();
    fe.Feed();
    auto newCount = fe.GetFeedCount();

    TEST_ASSERT_TRUE(newCount == (oldCount + 1));
}

void RUN_FEEDER_TESTS()
{
    RUN_TEST(should_feed_when_event_triggered);
}