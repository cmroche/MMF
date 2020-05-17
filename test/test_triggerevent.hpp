#pragma once

#include <unity.h>
#include "appconfig.h"
#include "triggerevent.h"

#include "test_util.hpp"

void should_fire_event_when_ticked(void) 
{
    auto fire = false;
    TriggerEvent<mock_millis> te(TRIGGER_EVENT_TIME, [](void* p)
    { 
        *((bool*)p) = true;
    }, &fire);

    mock_millis_elapse(TRIGGER_EVENT_TIME);
    te.Update();

    TEST_ASSERT_TRUE(fire);
}

void should_keep_firing_events_when_ticked(void) 
{
    auto fire = false;
    TriggerEvent<mock_millis> te(TRIGGER_EVENT_TIME, [](void* p)
    {
        *((bool*)p) = true;
    }, &fire);
    
    mock_millis_elapse(TRIGGER_EVENT_TIME);
    te.Update();
    fire = false;

    mock_millis_elapse(TRIGGER_EVENT_TIME);
    te.Update();
    
    TEST_ASSERT_TRUE(fire);
}

void RUN_TRIGGEREVENT_TESTS()
{
    RUN_TEST(should_fire_event_when_ticked);
    RUN_TEST(should_keep_firing_events_when_ticked);
}