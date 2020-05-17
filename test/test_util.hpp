#pragma once


unsigned long mock_cpu_ticks = 0;

void mock_millis_elapse(unsigned long ms)
{
    mock_cpu_ticks += ms;
}

unsigned long mock_millis()
{
    return mock_cpu_ticks;
}