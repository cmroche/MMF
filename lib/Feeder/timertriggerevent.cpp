#include "timetriggerevent.h"

#include <Arduino.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include "callback.h"

#include <type_traits>
#include <functional>

TimeTriggerEvent::TimeTriggerEvent(handler_t cb)
: _cb(cb)
{
    
}

void TimeTriggerEvent::Update()
{
    if (!_alarmsRegistered)
    {
        _alarmsRegistered = true;
        auto handler = GETCB(OnTick_t, TimeTriggerEvent)(std::bind(&TimeTriggerEvent::OnAlarm, this));
        _alarm.alarmRepeat(8, 0, 0, handler);
        _alarm.alarmRepeat(18, 0, 0, handler);
    }

    _alarm.delay(0);
}

void TimeTriggerEvent::OnAlarm() const
{
    tmElements_t elems;
    breakTime(now(), elems);
    String timeStr = String(monthShortStr(elems.Month)) + " " + elems.Day + " " + elems.Hour + ":" + elems.Minute + ":" + elems.Second;
    Serial.printf("Timer trigger: sending feed event %s\n", timeStr.c_str());
    
    if (_cb != nullptr) _cb();
}