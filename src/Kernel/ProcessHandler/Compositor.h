#pragma once

#include <stdint.h>

#include "STL/Graphics/Framebuffer.h"
#include "STL/Process/Process.h"

namespace Compositor
{   
    extern bool DrawMouse;

    void Init(STL::Framebuffer* Screenbuffer);

    void UpdateMouse(STL::MDATA MouseData);

    void RequestRender();

    void Update();
}