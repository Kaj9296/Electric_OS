#pragma once

#include "STL/Math/Point.h"
#include "STL/Graphics/Framebuffer.h"
#include "STL/String/String.h"
#include "STL/Process/Process.h"

namespace STL
{
    class Button 
    {
    public:

        ARGB Color;
        
        String Text;

        Point TopLeft;

        Point BottomRight;

        uint8_t Scale;

        void Draw(Framebuffer* Buffer);

        bool IsToggled(MDATA MouseInfo);

        bool IsPressed(MDATA MouseInfo);

        bool IsDown();

        Button() = default;

        Button(ARGB Color, const char* Text, Point TopLeft, Point BottomRight);

    private:

        bool Pressed = false;
    } ;
}