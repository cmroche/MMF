#pragma once

#include <TimeAlarms.h>

class TimeTriggerEvent
{
public:
    typedef void (*handler_t)(void);

    TimeTriggerEvent(handler_t cb);

    void Update();

private:
    handler_t _cb = nullptr;
    TimeAlarmsClass _alarm;

    bool _alarmsRegistered = false;

    void OnAlarm() const;
};