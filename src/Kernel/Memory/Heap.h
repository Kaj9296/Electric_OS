#pragma once

#include <stdint.h>

#define HEAP_START 0x100000000000
#define HEAP_STARTSIZE 0x10 * 4096

namespace Heap
{
    struct Segment
    {
        Segment* Next;
        uint64_t Size;
        bool Free;

        void* GetStart();

        void* GetEnd();

        void Split(uint64_t NewSize);
    };

    Segment* GetFirstSegment();

    void Init();

    uint64_t GetUsedSize();

    uint64_t GetFreeSize();

    uint64_t GetSegmentAmount();

    void* Allocate(uint64_t Size);

    void Free(void* Address);

    void Reserve(uint64_t Size);
}