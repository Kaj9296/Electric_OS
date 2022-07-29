#pragma once

#include "STL/Graphics/Framebuffer.h"
#include "STL/String/String.h"

namespace STL
{
    struct WindowFrame
    {
        void (*RenderFunction)(STL::Framebuffer*, STL::Point, STL::Point, STL::String&, bool);
        STL::Point TopLeftOffset;
        STL::Point BottomRightOffset;

        WindowFrame(void (*RenderFunction)(STL::Framebuffer*, STL::Point, STL::Point, STL::String&, bool), STL::Point TopLeftOffset, STL::Point BottomRightOffset);

        WindowFrame();
    };

    namespace WindowFrameRenderFunction
    {
        void SimpleFrame(STL::Framebuffer* Buffer, STL::Point ScreenPos, STL::Point WindowSize, STL::String& Title, bool IsFocused);
    }

    void InitWindowFrames();

    extern WindowFrame SimpleWindowFrame;
}