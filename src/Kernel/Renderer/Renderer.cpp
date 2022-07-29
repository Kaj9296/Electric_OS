#include "Renderer.h"

#include "STL/String/cstr.h"

#include "Kernel/PIT/PIT.h"
#include "Kernel/Memory/Heap.h"
#include "Kernel/Input/Mouse.h"

#include <stdint.h>

namespace Renderer
{
    STL::Framebuffer* Frontbuffer;

    STL::Point CursorPos;

    STL::ARGB Background;
    STL::ARGB Foreground;
    
    void Init(STL::Framebuffer* Screenbuffer)
    {
        Frontbuffer = Screenbuffer;

        CursorPos.X = 0;
        CursorPos.Y = 0;

        Background = STL::ARGB(0);
        Foreground = STL::ARGB(255);
    }

    void Clear()
    {
        Frontbuffer->Clear();
    }

    void PutChar(char chr, STL::Point Pos, uint8_t Scale)
    {
        Frontbuffer->PutChar(chr, Pos, Scale, Foreground, Background);
    }

    void Print(const char* str, uint8_t Scale)
    {
        Frontbuffer->Print(str, CursorPos, Scale, Foreground, Background);
    }

    void Print(char Chr, uint8_t Scale)
    {        
        Frontbuffer->PutChar(Chr, CursorPos, Scale, Foreground, Background);
    }

    STL::Point GetScreenSize()
    {
        return STL::Point(Frontbuffer->Width, Frontbuffer->Height);
    }
}
