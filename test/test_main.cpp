#include <unity.h>

#include "test_triggerevent.hpp"
#include "test_feeder.hpp"

void process()
{
    UNITY_BEGIN();
    RUN_TRIGGEREVENT_TESTS();
    RUN_FEEDER_TESTS();
    UNITY_END();
}

#ifdef ARDUINO
void setup() 
{
    delay(2000);
    process();
}

void loop() 
{
    // Don't like testing with a loop...
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif