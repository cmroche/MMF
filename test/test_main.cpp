#include <unity.h>
#include <TimeLib.h>

#include "test_timetriggerevent.hpp"
#include "test_mqtttriggerevent.hpp"
#include "test_feeder.hpp"

void process()
{
    SPI.begin();
    setTime(0, 0, 1, 1, 1, 2000); //h, m, s, d, m, y

    UNITY_BEGIN();
    RUN_TIMETRIGGEREVENT_TESTS();
    RUN_MQTTTRIGGEREVENT_TESTS();
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

}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif