#pragma once

#include <Arduino.h>
#include <AFMotor.h>

class Feeder
{
private:
    AF_Stepper& _stepper;
    unsigned long _feedCount = 0;

public:
    Feeder(AF_Stepper& stepper);
    ~Feeder() = default;

    Feeder(const Feeder&) = delete;
    Feeder(Feeder&&) = default;

    void Feed();
    unsigned long GetFeedCount() const { return _feedCount; }
};