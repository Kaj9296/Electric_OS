#include "PIT.h"

#include "Kernel/IO/IO.h"

namespace PIT
{
    volatile uint64_t Ticks = 0;

    uint16_t Divisor = 65535;

    void Sleep(double Seconds)
    {	
        uint64_t EndTicks = Ticks + GetFrequency() * Seconds;
        while (Ticks < EndTicks)
        {
            asm("HLT");
        }
    }

    uint64_t GetFrequency()
    {
        return PIT_FREQUENCY / Divisor;
    }

    void SetFrequency(uint64_t Frequency)
    {
        SetDivisor(PIT_FREQUENCY / Frequency);
    }

    void SetDivisor(uint16_t NewDivisor)
    {
        Divisor = NewDivisor;
        Ticks = 0;
        
        IO::OutByte(0x40, (uint8_t)Divisor);
        IO::Wait();
        IO::OutByte(0x40, (uint8_t)(Divisor >> 8));
    }

    void Tick()
    {
        Ticks = Ticks + 1;
    }
}