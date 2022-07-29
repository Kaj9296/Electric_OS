#include "ProcessHandler.h"
#include "Compositor.h"

#include "Programs/tty/tty.h"
#include "Kernel/Input/KeyBoard.h"

namespace ProcessHandler
{            
    Process* GrandFatherProcess = nullptr;
    Process* FocusedProcess = nullptr;
    Process* ActiveProcess = nullptr;

    void KeyBoardInterupt()
    {
        if (FocusedProcess != nullptr)
        {
            uint8_t Key = KeyBoard::GetKeyPress();
            FocusedProcess->SendMessage(STL::PROM::KEYPRESS, &Key);
        }
    }

    void MouseInterupt(STL::MDATA MouseData)
    {        
        Compositor::UpdateMouse(MouseData);

        if (GrandFatherProcess != nullptr)
        {
            GrandFatherProcess->ProcessMouseData(MouseData, STL::Point(0, 0));
        }
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