#include "UEFI.h"

namespace UEFI
{
    EfiRuntimeServices* RT;
    
    void Init(EfiRuntimeServices* In_RT)
    {
        RT = In_RT;
    }

    EfiRuntimeServices* GetRT()
    {
        return RT;
    }
}