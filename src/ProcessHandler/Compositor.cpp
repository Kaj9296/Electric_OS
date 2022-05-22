#include "Compositor.h"
#include "ProcessHandler.h"

#include "STL/Memory/Memory.h"

namespace Compositor
{            
    STL::Framebuffer* Frontbuffer;
    STL::Framebuffer Backbuffer;

    void SwapBuffers()
    {
        STL::CopyMemory(Backbuffer.Base, Frontbuffer->Base, Backbuffer.Size);
    }

    void Init(STL::Framebuffer* Screenbuffer)
    {
        Frontbuffer = Screenbuffer;
        Backbuffer = *Frontbuffer;
        
        Backbuffer.Base = (STL::ARGB*)STL::Malloc(Backbuffer.Size);
        Backbuffer.Clear();
    }

    void Update()
    {            
        if (ProcessHandler::GrandFatherProcess != nullptr)
        {
            ProcessHandler::GrandFatherProcess->Render(&Backbuffer);
        }
        SwapBuffers();
    }
}