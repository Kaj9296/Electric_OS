#include "SystemMenu.h"

#include "STL/System/System.h"
#include "STL/Graphics/Framebuffer.h"
#include "STL/String/cstr.h"
#include "STL/GUI/Button.h"

namespace SystemMenu
{        
    STL::Button TTYButton;
    STL::Button RestartButton;
    STL::Button ShutDownButton;

    void(*CurrentAnimation)(STL::Framebuffer*);
    uint64_t AnimationCounter = 0;

    inline void StartAnimation(void(*Animation)(STL::Framebuffer*))
    {
        AnimationCounter = 0;
        CurrentAnimation = Animation;
    }

    void OpenAnimation(STL::Framebuffer* Buffer)
    {
        Buffer->DrawRaisedRect(STL::Point(RAISED_WIDTH, RAISED_WIDTH), STL::Point(Buffer->Width - RAISED_WIDTH, Buffer->Height - RAISED_WIDTH), STL::ARGB(200));
    
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
            Info->Left = 1920 - 200 - 25;
            Info->Top = 50;
            Info->Width = 200;
            Info->Height = RAISED_WIDTH * 12 + 3 * (RAISED_WIDTH * 2 + 25);
            Info->Title = "SystemMenu";

            TTYButton = STL::Button(STL::ARGB(200), "To TTY", STL::Point(RAISED_WIDTH * 3, RAISED_WIDTH * 3), 
                                        STL::Point(Info->Width - RAISED_WIDTH * 3, RAISED_WIDTH * 3 + (RAISED_WIDTH * 2 + 25)));
            RestartButton = STL::Button(STL::ARGB(200), "Restart", STL::Point(RAISED_WIDTH * 3, RAISED_WIDTH * 6 + (RAISED_WIDTH * 2 + 25)), 
                                        STL::Point(Info->Width - RAISED_WIDTH * 3, RAISED_WIDTH * 6 + 2 * (RAISED_WIDTH * 2 + 25)));
            ShutDownButton = STL::Button(STL::ARGB(200), "Shut Down", STL::Point(RAISED_WIDTH * 3, RAISED_WIDTH * 9 + 2 * (RAISED_WIDTH * 2 + 25)), 
                                        STL::Point(Info->Width - RAISED_WIDTH * 3, RAISED_WIDTH * 9 + 3 * (RAISED_WIDTH * 2 + 25)));

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

            TTYButton.Draw(Buffer);
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
            STL::MDATA MouseInfo = *(STL::MDATA*)Input;

            if (TTYButton.IsToggled(MouseInfo))
            {
                STL::System("set drawmouse 0");
                return STL::PROR::RESET;
            }

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
        default:
        {

        }
        break;
        }

        return STL::PROR::SUCCESS;
    }
}