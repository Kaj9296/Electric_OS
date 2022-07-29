#include "WindowButton.h"

#include "STL/Process/Process.h"

namespace STL
{
    WindowButton::WindowButton(void(*RenderFunction)(STL::Framebuffer*, STL::Point, STL::Point), STL::PROR Request, STL::Point Pos, STL::Point Offset, STL::Point Size)
    {
        this->RenderFunction = RenderFunction;
        this->Request = Request;
        this->Pos = Pos;
        this->Offset = Offset;
        this->Size = Size;
    }   

    WindowButton::WindowButton()
    {        
        this->RenderFunction = nullptr;
        this->Request = STL::PROR::SUCCESS;
        this->Pos = STL::Point(0, 0);
        this->Offset = STL::Point(0, 0);
        this->Size = STL::Point(0, 0);
    }

    namespace WindowButtonRenderFunction
    {
        void SimpleCloseButton(STL::Framebuffer* Buffer, STL::Point ScreenPos, STL::Point WindowSize)
        {
            //Draw close button                    
            STL::Point ButtonPos = ScreenPos + STL::Point(WindowSize.X, 0) + STL::Point(-15 - 8, -15 - 8);                    
            Buffer->DrawRect(ButtonPos, ButtonPos + STL::Point(16, 16), STL::ARGB(200));
            Buffer->PutChar('X', ButtonPos + STL::Point(RAISED_WIDTH, 2), 1, STL::ARGB(0), STL::ARGB(200));
            Buffer->DrawRaisedRectEdge(ButtonPos, ButtonPos + STL::Point(16, 16));
        }
    }

    WindowButton SimpleCloseButton;

    void InitWindowButtons()
    {
        SimpleCloseButton = WindowButton(WindowButtonRenderFunction::SimpleCloseButton, STL::PROR::KILL, STL::Point(1, 0), STL::Point(-15 - 8, -15 - 8), STL::Point(16, 16));
    }
}