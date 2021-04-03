#pragma once

#include <TMCStepper.h>

class Feeder
{
private:
    TMC2130Stepper _stepper;
    unsigned long _feedCount = 0;
    unsigned long _feedSteps = 10000;

public:
    Feeder();

    void Feed();
    unsigned long GetFeedCount() const { return _feedCount; }
    void SetFeedSteps(unsigned long steps) { _feedSteps = steps; }
    unsigned long GetFeedSteps() const { return _feedSteps; }

    void InitDriver();
    bool ValidateDriverStatus();
    void Update();
};