#include "IO.h"

namespace IO
{
    void OutByte(uint16_t Port, uint8_t Value)
    {
        asm volatile ("OUTB %0, %1" : : "a"(Value), "Nd"(Port));
    }

    uint8_t InByte(uint16_t Port)
    {
        uint8_t ReturnValue;
        asm volatile ("INB %1, %0" : "=a"(ReturnValue) : "Nd"(Port));
        return ReturnValue;
    }

    void Wait()
    {
        asm volatile("OUTB %%al, $0x80" : : "a"(0));
    }
}
