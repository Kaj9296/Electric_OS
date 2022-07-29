#pragma once

#include "STL/Graphics/Font.h"
#include "STL/Graphics/ARGB.h"
#include "STL/Graphics/Framebuffer.h"
#include "STL/Math/Math.h"
#include "STL/Memory/Memory.h"

namespace Renderer
{    
    extern STL::Point CursorPos;
    extern STL::ARGB Background;
    extern STL::ARGB Foreground;

    void Init(STL::Framebuffer* Screenbuffer);

    void Clear();

    void Print(const char* str, uint8_t Scale = 1);

    void Print(char Chr, uint8_t Scale = 1);

    STL::Point GetScreenSize();
}
