#pragma once

#include <unity.h>

#include "feeder.h"
#include "timetriggerevent.h"

#include "test_util.hpp"

void should_feed_when_event_triggered(void) 
{
    Feeder fe;
    auto oldCount = fe.GetFeedCount();
    fe.Feed();
    auto newCount = fe.GetFeedCount();

    TEST_ASSERT_TRUE(newCount == (oldCount + 1));
}

void should_validate_driver(void)
{
    Feeder fe;
    fe.InitDriver();
    fe.Feed();
    auto result = fe.ValidateDriverStatus();

    TEST_ASSERT_TRUE(result);
}

void RUN_FEEDER_TESTS()
{
    RUN_TEST(should_feed_when_event_triggered);
    RUN_TEST(should_validate_driver);
}