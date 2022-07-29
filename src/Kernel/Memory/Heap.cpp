#include "Heap.h"

#include "Kernel/Memory/Paging/PageAllocator.h"
#include "Kernel/Memory/Paging/PageTable.h"

namespace Heap
{
    Segment* FirstSegment = nullptr;
    Segment* LastSegment = nullptr;

    void* Segment::GetStart()
    {
        return (void*)((uint64_t)this + sizeof(Segment));
    }

    void* Segment::GetEnd()
    {
        return (void*)((uint64_t)this + sizeof(Segment) + this->Size);
    }

    void Segment::Split(uint64_t NewSize)
    {
        uint64_t SplitSize = this->Size - NewSize - sizeof(Segment);

        if (this->Size < 64 || SplitSize < 4096)
        {
            return; 
        }

        Segment* NewSegment = (Segment*)((uint64_t)this->GetStart() + NewSize);
        NewSegment->Next = this->Next;
        NewSegment->Size = SplitSize;
        NewSegment->Free = true;

        this->Next = NewSegment;
        this->Size = NewSize - sizeof(Segment);
    }

    Segment* GetFirstSegment()
    {
        return FirstSegment;
    }

    void Init()
    {
        FirstSegment = (Segment*)HEAP_START;
        void* Address = FirstSegment;
        for (uint64_t i = 0; i < (HEAP_STARTSIZE + sizeof(Segment)) / 4096 + 1; i++)
        {
            PageTableManager::MapAddress(Address, PageAllocator::RequestPage());
            Address = (void*)((uint64_t)Address + 4096);
        }

        FirstSegment->Size = HEAP_STARTSIZE;
        FirstSegment->Next = nullptr;
        FirstSegment->Free = true;
        
        LastSegment = FirstSegment;
    }

    uint64_t GetUsedSize()
    {
        uint64_t UsedSize = 0;

        Segment* CurrentSegment = FirstSegment;
        while (true)
        {
            if (!CurrentSegment->Free)
            {
                UsedSize += CurrentSegment->Size;
            }

            if (CurrentSegment->Next == nullptr)
            {
                break;
            }
            CurrentSegment = CurrentSegment->Next;
        }

        return UsedSize;
    }

    uint64_t GetFreeSize()
    {
        uint64_t FreeSize = 0;

        Segment* CurrentSegment = FirstSegment;
        while (true)
        {
            if (CurrentSegment->Free)
            {
                FreeSize += CurrentSegment->Size;
            }

            if (CurrentSegment->Next == nullptr)
            {
                break;
            }
            CurrentSegment = CurrentSegment->Next;
        }

        return FreeSize;
    }

    uint64_t GetSegmentAmount()
    {
        uint64_t SegmentAmount = 0;

        Segment* CurrentSegment = FirstSegment;
        while (true)
        {
            SegmentAmount++;

            if (CurrentSegment->Next == nullptr)
            {
                break;
            }
            CurrentSegment = CurrentSegment->Next;
        }

        return SegmentAmount;
    }

    void* Allocate(uint64_t Size)
    {
        if (Size == 0)
        {
            return nullptr;
        }

        Size = Size + (64 - (Size % 64));

        Segment* CurrentSegment = FirstSegment;
        while (true)
        {
            if (CurrentSegment->Free)
            {
                if (CurrentSegment->Size == Size)
                {
                    CurrentSegment->Free = false;
                    return CurrentSegment->GetStart();
                }
                else if (CurrentSegment->Size > Size)
                {
                    CurrentSegment->Split(Size);
                    CurrentSegment->Free = false;
                    return CurrentSegment->GetStart();
                }
            }

            if (CurrentSegment->Next == nullptr)
            {
                break;
            }
            CurrentSegment = CurrentSegment->Next;
        }

        Reserve(Size);
        return Allocate(Size);
    }

    void Free(void* Address)
    {
        Segment* segment = (Segment*)((uint64_t)Address - sizeof(Segment));
        segment->Free = true;

        Segment* CurrentSegment = FirstSegment;
        while (true)
        {
            if (CurrentSegment == nullptr || CurrentSegment->Next == nullptr)
            {
                break;
            }

            if (CurrentSegment->Free && CurrentSegment->Next->Free)
            {
                CurrentSegment->Size += CurrentSegment->Next->Size + sizeof(Segment);
                CurrentSegment->Next = CurrentSegment->Next->Next;
            }

            CurrentSegment = CurrentSegment->Next;
        }
    }

    void Reserve(uint64_t Size)
    {   
        Size = Size + (4096 - (Size % 4096));
        Segment* NewSegment = (Segment*)LastSegment->GetEnd();

        void* Address = NewSegment;
        for (uint64_t i = 0; i < (Size + sizeof(Segment)) / 4096 + 1; i++)
        {
            PageTableManager::MapAddress(Address, PageAllocator::RequestPage());
            Address = (void*)((uint64_t)Address + 4096);
        }

        NewSegment->Size = Size;
        NewSegment->Next = nullptr;
        NewSegment->Free = true;

        LastSegment->Next = NewSegment;
        
        LastSegment = NewSegment;
    }
}