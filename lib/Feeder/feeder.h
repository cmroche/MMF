#pragma once

#include <TMCStepper.h>

class Feeder
{
private:
    TMC2130Stepper _stepper;
    unsigned long _feedCount = 0;

public:
    Feeder(/*AF_Stepper& stepper*/);

    void Feed();
    unsigned long GetFeedCount() const { return _feedCount; }

    void InitDriver();
    bool ValidateDriverStatus();
    void Update();
};