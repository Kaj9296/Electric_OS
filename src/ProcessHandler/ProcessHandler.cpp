#include "ProcessHandler.h"
#include "Compositor.h"

#include "Programs/tty/tty.h"
#include "Input/KeyBoard.h"

namespace ProcessHandler
{            
    Process* GrandFatherProcess;
    Process* FocusedProcess;
    Process* ActiveProcess;

    void KeyBoardInterupt()
    {
        if (FocusedProcess != nullptr)
        {
            uint8_t Key = KeyBoard::GetKeyPress();
            FocusedProcess->SendMessage(STL::PROM::KEYPRESS, &Key);
        }
    }

    void MouseInterupt()
    {

    }

    void PITInterupt(uint64_t Ticks)
    {
        if (GrandFatherProcess != nullptr)
        {
            GrandFatherProcess->SendFamilyMessage(STL::PROM::TICK, &Ticks);
        }
    }

    void Loop()
    {                
        new Process(tty::Procedure, nullptr);
        
        while (true) 
        {   
            if (GrandFatherProcess != nullptr)
            {
                GrandFatherProcess->HandleRequest();
            }

            Compositor::Update();

            asm("HLT");
        }
    }
}