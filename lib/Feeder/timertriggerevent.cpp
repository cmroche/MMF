#include <Arduino.h>

#include "timetriggerevent.h"

TimeTriggerEvent::TimeTriggerEvent(handler_t cb)
: _cb(cb)
{
    _alarm.alarmRepeat(8, 0, 0, _cb);
    _alarm.alarmRepeat(18, 0, 0, _cb);
}

void TimeTriggerEvent::Update()
{
    _alarm.delay(0);
}