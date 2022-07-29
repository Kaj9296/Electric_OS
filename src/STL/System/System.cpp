#include "System.h"

#include "Kernel/System/System.h"

void* operator new(uint64_t Size) 
{
    return STL::Malloc(Size);
}

//Delete operators cause a page fault for an unknown reason.
/*void operator delete(void* Address, uint64_t Size) 
{
    return STL::Free(Address);
}

void operator delete(void* Address) 
{
    return STL::Free(Address);
}*/

namespace STL
{    
    const char* System(const char* Command)
    {
        return (const char*)System::Call(SYSCALL_SYSTEM, Command);
    }

    void* Malloc(uint64_t Size)
    {
        return (void*)System::Call(SYSCALL_MALLOC, Size);
    }

    void Free(void* Memory)
    {
        System::Call(SYSCALL_FREE, Memory);
    }
}