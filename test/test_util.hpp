#pragma once


unsigned long mock_cpu_ticks = 60L * 60L * 1000L;

void mock_millis_elapse(unsigned long ms)
{
    mock_cpu_ticks += ms;
}

unsigned long mock_millis()
{
    return mock_cpu_ticks;
}