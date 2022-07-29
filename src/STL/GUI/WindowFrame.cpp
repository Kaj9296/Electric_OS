#include "WindowFrame.h"

/*
#define FRAME_OFFSET STL::Point(0, 30)
#define CLOSE_BUTTON_SIZE STL::Point(16, 16)
#define CLOSE_BUTTON_OFFSET STL::Point(((FRAME_OFFSET.X - CLOSE_BUTTON_SIZE.X) / 2 - CLOSE_BUTTON_SIZE.X), -FRAME_OFFSET.Y / 2 - CLOSE_BUTTON_SIZE.Y / 2)
#define MINIMIZE_BUTTON_SIZE STL::Point(16, 16)
#define MINIMIZE_BUTTON_OFFSET CLOSE_BUTTON_OFFSET - STL::Point(16 + RAISED_WIDTH * 3, 0)
*/

namespace STL
{
    WindowFrame::WindowFrame(void (*RenderFunction)(STL::Framebuffer*, STL::Point, STL::Point, STL::String&, bool), STL::Point TopLeftOffset, STL::Point BottomRightOffset)
    {
        this->RenderFunction = RenderFunction;
        this->TopLeftOffset = TopLeftOffset;
        this->BottomRightOffset = BottomRightOffset;
    }

    WindowFrame::WindowFrame()
    {
        this->RenderFunction = nullptr;
        this->TopLeftOffset = STL::Point(0, 0);
        this->BottomRightOffset = STL::Point(0, 0);
    }

    namespace WindowFrameRenderFunction
    {
        void SimpleFrame(STL::Framebuffer* Buffer, STL::Point ScreenPos, STL::Point WindowSize, STL::String& Title, bool IsFocused)
        {
            const STL::Point FrameOffset = STL::Point(0, 30);

            STL::ARGB Background;
            STL::ARGB Foreground;
            if (IsFocused)
            {
                Background = STL::ARGB(255, 14, 0, 135);            
                Foreground = STL::ARGB(255);
            }
            else
            {
                Background = STL::ARGB(128);            
                Foreground = STL::ARGB(192);            
            }

            //Draw topbar
            Buffer->DrawRaisedRectEdge(ScreenPos - FrameOffset, ScreenPos + WindowSize);
            Buffer->DrawRect(ScreenPos - FrameOffset, ScreenPos + STL::Point(WindowSize.X, 0), Background);

            /*//Draw close button                    
            STL::Point CloseButtonPos = CLOSE_BUTTON_OFFSET + STL::Point(WindowSize.X, 0) + ScreenPos;                    
            Buffer->DrawRect(CloseButtonPos, CloseButtonPos + CLOSE_BUTTON_SIZE, STL::ARGB(200));
            Buffer->PutChar('X', CloseButtonPos + STL::Point(RAISED_WIDTH, 2), 1, STL::ARGB(0), STL::ARGB(200));
            Buffer->DrawRaisedRectEdge(CloseButtonPos, CloseButtonPos + CLOSE_BUTTON_SIZE);

            //Draw minimize button                    
            STL::Point MinimizeButtonPos = MINIMIZE_BUTTON_OFFSET + STL::Point(WindowSize.X, 0) + ScreenPos;
            Buffer->DrawRect(MinimizeButtonPos, MinimizeButtonPos + MINIMIZE_BUTTON_SIZE, STL::ARGB(200));
            Buffer->PutChar('_', MinimizeButtonPos + STL::Point(RAISED_WIDTH, 0), 1, STL::ARGB(0), STL::ARGB(200));
            Buffer->DrawRaisedRectEdge(MinimizeButtonPos, MinimizeButtonPos + MINIMIZE_BUTTON_SIZE);*/

            //Print Title
            STL::Point TextPos = ScreenPos + STL::Point(RAISED_WIDTH * 2, -FrameOffset.Y / 2 - 8);
            Buffer->Print(Title.cstr(), TextPos, 1, Foreground, Background); 
        }        
    }

    WindowFrame SimpleWindowFrame;

    void InitWindowFrames()
    {
        SimpleWindowFrame = WindowFrame(WindowFrameRenderFunction::SimpleFrame, STL::Point(0, 30), STL::Point(RAISED_WIDTH, RAISED_WIDTH));
    }

}