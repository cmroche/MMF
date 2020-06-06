#pragma once

class Feeder
{
private:
    //AF_Stepper& _stepper;
    unsigned long _feedCount = 0;

public:
    Feeder(/*AF_Stepper& stepper*/);

    void Feed();
    unsigned long GetFeedCount() const { return _feedCount; }
};