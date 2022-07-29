#include "PageAllocator.h"

#include "STL/String/cstr.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

namespace PageAllocator
{
    uint8_t* PageStatusMap;
    uint64_t PageAmount;

    uint64_t FirstFreePage = 0;

    bool GetPageStatus(uint64_t Index)
    {
        return (PageStatusMap[Index / 8] >> (Index % 8)) & 1;
    }

    void SetPageStatus(uint64_t Index, bool Status)
    {
        if (Status)
        {
            PageStatusMap[Index / 8] |= 1 << (Index % 8);
        }
        else
        {
            PageStatusMap[Index / 8] &= ~(1 << (Index % 8));
        }
    }

    void Init(EFI_MEMORY_MAP* MemoryMap, STL::Framebuffer* ScreenBuffer)
    {   
        PageAmount = 0;
        for (uint64_t i = 0; i < MemoryMap->Size / MemoryMap->DescSize; i++)
        {
            EFI_MEMORY_DESCRIPTOR* Desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MemoryMap->Base + (i * MemoryMap->DescSize));
            PageAmount += Desc->NumberOfPages;
        }

        void* LargestFreeSegment = nullptr;
        uint64_t LargestFreeSegmentSize = 0;
        for (uint64_t i = 0; i < MemoryMap->Size / MemoryMap->DescSize; i++)
        {
            EFI_MEMORY_DESCRIPTOR* Desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MemoryMap->Base + (i * MemoryMap->DescSize));
            if (Desc->Type == (uint32_t)EFI_MEMORY_TYPE::EfiConventionalMemory && LargestFreeSegmentSize < Desc->NumberOfPages * 4096)
            {
                LargestFreeSegment = Desc->PhysicalStart;
                LargestFreeSegmentSize = Desc->NumberOfPages * 4096;
            }
        }
        PageStatusMap = (uint8_t*)LargestFreeSegment; 
        for (uint64_t i = 0; i < PageAmount / 8; i++)
        {
            PageStatusMap[i] = false;
        }
        LockPages(PageStatusMap, (PageAmount / 4096 / 8) + 1);

        for (uint64_t i = 0; i < MemoryMap->Size / MemoryMap->DescSize; i++)
        {
            EFI_MEMORY_DESCRIPTOR* Desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MemoryMap->Base + (i * MemoryMap->DescSize));
            if (Desc->Type != (uint32_t)EFI_MEMORY_TYPE::EfiConventionalMemory)
            {
                LockPages(Desc->PhysicalStart, Desc->NumberOfPages);
            }
        }
        LockPages(&_KernelStart, ((uint64_t)&_KernelEnd - (uint64_t)&_KernelStart) / 4096 + 1);
           
        LockPages(ScreenBuffer->Base, ScreenBuffer->Size / 4096 + 1);
    }

    uint64_t GetFreePages()
    {
        uint64_t FreeMemory = 0;
        for (uint64_t i = 0; i < PageAmount; i++)
        {
            FreeMemory += !GetPageStatus(i);
        }
        return FreeMemory;
    }

    uint64_t GetLockedPages()
    {
        uint64_t LockedMemory = 0;
        for (uint64_t i = 0; i < PageAmount; i++)
        {
            LockedMemory += GetPageStatus(i);
        }
        return LockedMemory;
    }

    uint64_t GetTotalPages()
    {
        return PageAmount;
    }

    void* RequestPage()
    {
        for (uint64_t i = FirstFreePage; i < PageAmount; i++)
        {
            if (!GetPageStatus(i))
            {
                FirstFreePage = i + 1;
                return LockPage((void*)(i * 4096));
            }
        }

        return nullptr;
    }

    void* LockPage(void* Address)
    {
        uint64_t PageIndex = (uint64_t)Address / 4096;
        if (PageIndex > PageAmount)
        {
            return nullptr;
        }
        SetPageStatus(PageIndex, true);

        return Address;
    }

    void* FreePage(void* Address)
    {
        uint64_t PageIndex = (uint64_t)Address / 4096;
        if (PageIndex > PageAmount)
        {
            return nullptr;
        }
        SetPageStatus(PageIndex, false);
        if (FirstFreePage > PageIndex)
        {
            FirstFreePage = PageIndex;
        }

        return Address;
    }

    void LockPages(void* Address, uint64_t Count)
    {
        for (uint64_t i = 0; i < Count; i++)
        {
            LockPage((void*)((uint64_t)Address + i * 4096));
        }
    }

    void FreePages(void* Address, uint64_t Count)
    {
        for (uint64_t i = 0; i < Count; i++)
        {
            FreePage((void*)((uint64_t)Address + i * 4096));
        }
    }
}