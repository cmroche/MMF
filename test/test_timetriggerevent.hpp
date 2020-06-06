#pragma once

#include <unity.h>
#include <functional>
#include <type_traits>

#include "appconfig.h"
#include "timetriggerevent.h"

#include "test_util.hpp"

void should_fire_event_when_ticked(void) 
{
    RESET_TIME;

    struct Handler_t
    {
        bool fire = false;
        void cb()
        {
            fire = true;
        }
    } Handler;

    auto handler = GETCB(TimeTriggerEvent::handler_t, Handler_t)(std::bind(&Handler_t::cb, &Handler));
    TimeTriggerEvent te(handler);

    handler();

    SET_MORNING_TRIGGER_TIME;

    te.Update();

    TEST_ASSERT_TRUE(Handler.fire);
}

void should_keep_firing_events_when_ticked(void) 
{
    RESET_TIME;

    struct Handler_t
    {
        bool fire = false;
        void cb()
        {
            fire = true;
        }
    } Handler;

    auto handler = GETCB(TimeTriggerEvent::handler_t, Handler_t)(std::bind(&Handler_t::cb, &Handler));
    TimeTriggerEvent te(handler);
    
    SET_MORNING_TRIGGER_TIME;

    te.Update();
    Handler.fire = false;

    SET_EVENING_TRIGGER_TIME;

    te.Update();
    TEST_ASSERT_TRUE(Handler.fire);
}

void RUN_TIMETRIGGEREVENT_TESTS()
{
    Serial.write("Start TimeTriggerEvent unit tests\n");
    RUN_TEST(should_fire_event_when_ticked);
    RUN_TEST(should_keep_firing_events_when_ticked);
}