#pragma once

#include <stdint.h>
#include "STL/Graphics/Framebuffer.h"

struct PageDirEntry
{
    bool Present : 1;
    bool ReadWrite : 1;
    bool UserSuper : 1;
    bool WriteThrough : 1;
    bool CacheDisabled : 1;
    bool Accessed : 1;
    bool Ignore0 : 1; 
    bool LargerPages : 1;
    bool Ignore1 : 1;

    uint8_t Available : 3;

    uint64_t Address : 52;
};

struct PageTable 
{ 
    PageDirEntry Entries[512];
}__attribute__((aligned(0x1000)));

namespace PageTableManager
{
    void Init(STL::Framebuffer* ScreenBuffer);

    void MapAddress(void* VirtualAddress, void* PhysicalAddress);
}