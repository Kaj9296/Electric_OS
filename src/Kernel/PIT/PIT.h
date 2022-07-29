#pragma once

#include <stdint.h>

#define PIT_FREQUENCY 1193182

namespace PIT
{
    extern volatile uint64_t Ticks;

    void Sleep(double Seconds);

    uint64_t GetFrequency();

    void SetFrequency(uint64_t Frequency);

    void SetDivisor(uint16_t NewDivisor);

    void Tick();
}