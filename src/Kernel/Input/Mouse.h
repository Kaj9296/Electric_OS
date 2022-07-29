#pragma once

#include "STL/Math/Point.h"

namespace Mouse
{
    void InitPS2();

    void HandleMouseByte(uint8_t MouseByte);
}