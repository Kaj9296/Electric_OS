#pragma once

#include "STL/Graphics/Framebuffer.h"

namespace STL
{
    enum class PROR;

    struct WindowButton
    {
        void (*RenderFunction)(STL::Framebuffer*, STL::Point, STL::Point);
        STL::PROR Request;
        STL::Point Pos; // Uses a scale from 0-1 across both axis
        STL::Point Offset; // A pixel offset from Pos.
        STL::Point Size;

        WindowButton(void (*RenderFunction)(STL::Framebuffer*, STL::Point, STL::Point), STL::PROR Request, STL::Point Pos, STL::Point Offset, STL::Point Size);

        WindowButton();
    };

    namespace WindowButtonRenderFunction
    {
        void SimpleCloseButton(STL::Framebuffer* Buffer, STL::Point ScreenPos, STL::Point WindowSize);
    }

    void InitWindowButtons();

    extern WindowButton SimpleCloseButton;
}