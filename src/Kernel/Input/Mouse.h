#pragma once

#include <stdint.h>

namespace Mouse
{
    void InitPS2();

    void HandleMouseByte(uint8_t MouseByte);
}