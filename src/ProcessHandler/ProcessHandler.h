#pragma once

#include "Process.h"

#include "STL/Process/Process.h"
#include "STL/List/List.h"

namespace ProcessHandler    
{               
    /// <summary>
    /// The process at the base of the tree.
    /// </summary>
    extern Process* GrandFatherProcess;

    /// <summary>
    /// The process that was last created or clicked.
    /// </summary>    
    extern Process* FocusedProcess;

    /// <summary>
    /// The process that is currently in control.
    /// </summary>       
    extern Process* ActiveProcess;

    void KeyBoardInterupt();

    void MouseInterupt();

    void PITInterupt(uint64_t Ticks);

    void Loop();   
}