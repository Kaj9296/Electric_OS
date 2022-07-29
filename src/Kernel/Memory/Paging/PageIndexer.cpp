#include "PageIndexer.h"

PageIndexer::PageIndexer(uint64_t VirtualAddress)
{
    VirtualAddress >>= 12;
    P = VirtualAddress & 0x1ff;

    VirtualAddress >>= 9;
    PT = VirtualAddress & 0x1ff;

    VirtualAddress >>= 9;
    PD = VirtualAddress & 0x1ff;

    VirtualAddress >>= 9;
    PDP = VirtualAddress & 0x1ff;
}