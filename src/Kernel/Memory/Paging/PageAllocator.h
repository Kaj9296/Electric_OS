#pragma once

#include "STL/Graphics/Framebuffer.h"
#include "STL/Graphics/Font.h"

#include "Kernel/Memory/efiMemory.h"

#include <stdint.h>

namespace PageAllocator
{    
    extern uint64_t PageAmount;

    void Init(EFI_MEMORY_MAP* MemoryMap);

    void* RequestPage();

    uint64_t GetFreePages();

    uint64_t GetLockedPages();

    uint64_t GetTotalPages();

    void* LockPage(void* Address);

    void* FreePage(void* Address);

    void LockPages(void* Address, uint64_t Count);

    void FreePages(void* Address, uint64_t Count);
}