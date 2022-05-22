#pragma once

#include <stdint.h>

#include "STL/Graphics/Framebuffer.h"

namespace Compositor
{
    void Init(STL::Framebuffer* Screenbuffer);

    void Update();
}