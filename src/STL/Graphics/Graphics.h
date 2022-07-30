#pragma once

#include "Font.h"

#include <stdint.h>

namespace STL
{    
    void AddFont(PSF_FONT* NewFont, const char* FontName);

    bool SetFont(const char* FontName);

    const PSF_FONT* GetSelectedFont();
}