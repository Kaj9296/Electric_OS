#include "PageTable.h"
#include "PageIndexer.h"
#include "PageAllocator.h"

#include "STL/Memory/Memory.h"

namespace PageTableManager
{
    PageTable* PML4;
    
    void Init(STL::Framebuffer* ScreenBuffer)
    {
        PML4 = (PageTable*)PageAllocator::RequestPage();
        STL::SetMemory(PML4, 0, 4096);

        for (uint64_t i = 0; i < ScreenBuffer->Size + 4096; i += 4096)
        {
            MapAddress((void*)((uint64_t)ScreenBuffer->Base + i), (void*)((uint64_t)ScreenBuffer->Base + i));
        }

        for (uint64_t i = 0; i < PageAllocator::PageAmount; i++)
        {
            MapAddress((void*)(i * 4096), (void*)(i * 4096));
        }

        asm ("mov %0, %%cr3" : : "r" (PML4));
    }

    void MapAddress(void* VirtualAddress, void* PhysicalAddress)
    {
        PageIndexer Indexer = PageIndexer((uint64_t)VirtualAddress);
        PageDirEntry PDE;

        PDE = PML4->Entries[Indexer.PDP];
        PageTable* PDP;
        if (!PDE.Present)
        {
            PDP = (PageTable*)PageAllocator::RequestPage();
            STL::SetMemory(PDP, 0, 0x1000);
            PDE.Address = (uint64_t)PDP >> 12;
            PDE.Present = true;
            PDE.ReadWrite = true;
            PML4->Entries[Indexer.PDP] = PDE;
        }
        else
        {
            PDP = (PageTable*)((uint64_t)PDE.Address << 12);
        }
        
        
        PDE = PDP->Entries[Indexer.PD];
        PageTable* PD;
        if (!PDE.Present)
        {
            PD = (PageTable*)PageAllocator::RequestPage();
            STL::SetMemory(PD, 0, 0x1000);
            PDE.Address = (uint64_t)PD >> 12;
            PDE.Present = true;
            PDE.ReadWrite = true;
            PDP->Entries[Indexer.PD] = PDE;
        }
        else
        {
            PD = (PageTable*)((uint64_t)PDE.Address << 12);
        }

        PDE = PD->Entries[Indexer.PT];
        PageTable* PT;
        if (!PDE.Present)
        {
            PT = (PageTable*)PageAllocator::RequestPage();
            STL::SetMemory(PT, 0, 0x1000);
            PDE.Address = (uint64_t)PT >> 12;
            PDE.Present = true;
            PDE.ReadWrite = true;
            PD->Entries[Indexer.PT] = PDE;
        }
        else
        {
            PT = (PageTable*)((uint64_t)PDE.Address << 12);
        }

        PDE = PT->Entries[Indexer.P];
        PDE.Address = (uint64_t)PhysicalAddress >> 12;
        PDE.Present = true;
        PDE.ReadWrite = true;
        PT->Entries[Indexer.P] = PDE;
    }
}