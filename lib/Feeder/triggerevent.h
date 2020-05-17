#pragma once

#include "timer.h"

template<unsigned long (*time_func)() = millis>
class TriggerEvent
{
public:
    typedef void (*handler_t)(void* opaque);

    void Update();

    TriggerEvent(uint32_t delay_seconds, handler_t cb, void* opaque);
    TriggerEvent(const TriggerEvent&) = delete;
    TriggerEvent(TriggerEvent&&) = default;
    ~TriggerEvent();

private:
    void* _opaque = nullptr;
    handler_t _cb = nullptr;

    typedef Timer<1, time_func> TimerType;
    TimerType _timer;
    typename TimerType::Task _task;
};

template<unsigned long (*time_func)()>
TriggerEvent<time_func>::TriggerEvent(uint32_t delay_seconds, handler_t cb, void* opaque)
{
    _cb = cb;
    _opaque = opaque;
    _task = _timer.every(delay_seconds, [](void* self) -> bool 
    { 
        auto te = (TriggerEvent*)self;
        if (te->_cb != nullptr)
            te->_cb(te->_opaque);

        return true;
    }, this);
}

template<unsigned long (*time_func)()>
TriggerEvent<time_func>::~TriggerEvent()
{
    _timer.cancel(_task);
}

template<unsigned long (*time_func)()>
void TriggerEvent<time_func>::Update()
{
    _timer.tick();
}