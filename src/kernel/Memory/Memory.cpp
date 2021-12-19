#include "Memory.h"

namespace Memory
{
    uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize)
    {
        static uint64_t memorySizeBytes = 0;
        if (memorySizeBytes > 0) return memorySizeBytes;

        for (int i = 0; i < mMapEntries; i++)
        {
            EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
            memorySizeBytes += desc->NumberOfPages * 4096;
        }

        return memorySizeBytes;
    }

    void SetMemory(void* Start, uint8_t Value, uint64_t Num)
    {
        for (uint64_t i = 0; i < Num; i++)
        {
            *(uint8_t*)((uint64_t)Start + i) = Value;
        }
    }

    void Copy(void* Source, void* Dest, uint64_t Count)
    {
        while (Count-- > 0)
        {
            *(uint8_t*)Dest++ = *(uint8_t*)Source++;
        }
    }

    void Copy64(void* Source, void* Dest, uint64_t Count)
    {
        while (Count -= 8 > 0)
        {
            *(uint64_t*)Dest++ = *(uint64_t*)Source++;
        }
    }
}