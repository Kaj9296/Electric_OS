#include "SystemMenu.h"

#include "STL/System/System.h"
#include "STL/Graphics/Framebuffer.h"
#include "STL/String/cstr.h"
#include "STL/GUI/Button.h"

namespace SystemMenu
{
    STL::Button ShutDownButton;
    STL::Button RestartButton;

    void(*CurrentAnimation)(STL::Framebuffer*);
    uint64_t AnimationCounter = 0;

    inline void StartAnimation(void(*Animation)(STL::Framebuffer*))
    {
        AnimationCounter = 0;
        CurrentAnimation = Animation;
    }

    void DrawEntry(STL::Framebuffer* Buffer, uint64_t Index, const char* Text)
    {
        Buffer->DrawSunkenRect(STL::Point(RAISEDWIDTH * 3, RAISEDWIDTH * (3 + Index) + Index * (RAISEDWIDTH * 2 + 25)), STL::Point(Buffer->Width - RAISEDWIDTH * 3, RAISEDWIDTH * (3 + Index) + (1 + Index) * (RAISEDWIDTH * 2 + 25)), STL::ARGB(200));
    }

    void OpenAnimation(STL::Framebuffer* Buffer)
    {
        Buffer->DrawRaisedRect(STL::Point(RAISEDWIDTH, RAISEDWIDTH), STL::Point(Buffer->Width - RAISEDWIDTH, Buffer->Height - RAISEDWIDTH), STL::ARGB(200));
    
        StartAnimation(nullptr);
    }

    STL::PROR Procedure(STL::PROM Message, STL::PROI Input)
    {
        switch(Message)
        {
        case STL::PROM::INIT:
        {
            STL::PINFO* Info = (STL::PINFO*)Input;
            Info->Type = STL::PROT::FRAMELESSWINDOW;
            Info->Depth = 1;
            Info->Left = 1920 - 200 - 50;
            Info->Top = 50;
            Info->Width = 200;
            Info->Height = RAISEDWIDTH * 9 + 2 * (RAISEDWIDTH * 2 + 25);

            RestartButton = STL::Button(STL::ARGB(200), "Restart", STL::Point(RAISEDWIDTH * 3, RAISEDWIDTH * 3), 
                                        STL::Point(Info->Width - RAISEDWIDTH * 3, RAISEDWIDTH * 3 + (RAISEDWIDTH * 2 + 25)));
            ShutDownButton = STL::Button(STL::ARGB(200), "Shut Down", STL::Point(RAISEDWIDTH * 3, RAISEDWIDTH * 6 + (RAISEDWIDTH * 2 + 25)), 
                                        STL::Point(Info->Width - RAISEDWIDTH * 3, RAISEDWIDTH * 6 + 2 * (RAISEDWIDTH * 2 + 25)));

            StartAnimation(OpenAnimation);
        }
        break;
        case STL::PROM::DRAW:
        {
            STL::Framebuffer* Buffer = (STL::Framebuffer*)Input;

            if (CurrentAnimation != nullptr)
            {
                CurrentAnimation(Buffer);
                AnimationCounter++;

                if (CurrentAnimation == nullptr)
                {
                    return STL::PROR::DRAW;
                }
            }   

            RestartButton.Draw(Buffer);
            ShutDownButton.Draw(Buffer);
        }
        break;
        case STL::PROM::TICK:
        {            
            if (CurrentAnimation != nullptr)
            {
                return STL::PROR::DRAW;
            }
        }
        break;
        case STL::PROM::MOUSE:
        {
            STL::MINFO MouseInfo = *(STL::MINFO*)Input;

            if (RestartButton.IsToggled(MouseInfo))
            {
                STL::System("restart");
            }
            
            if (ShutDownButton.IsToggled(MouseInfo))
            {
                STL::System("shutdown");
            }

            return STL::PROR::DRAW;
        }
        break;
        }

        return STL::PROR::SUCCESS;
    }
}