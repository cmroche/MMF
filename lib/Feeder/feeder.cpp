#include "feeder.h"

Feeder::Feeder(/*AF_Stepper& stepper*/)
//: _stepper(stepper)
{
    // Nothing else to do here, we assume the stepper is already initialized and functional
}

void Feeder::Feed()
{
    ++_feedCount;

    // TODO: Trigger motor
}