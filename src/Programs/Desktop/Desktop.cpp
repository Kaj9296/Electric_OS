#include "Desktop.h"

#include "STL/System/System.h"
#include "STL/Graphics/Framebuffer.h"
#include "STL/String/cstr.h"

#include "Programs/Topbar/Topbar.h"

namespace Desktop
{
    void(*CurrentAnimation)(STL::Framebuffer*);
    uint64_t AnimationCounter = 0;

    inline void StartAnimation(void(*Animation)(STL::Framebuffer*))
    {
        AnimationCounter = 0;
        CurrentAnimation = Animation;
    }

    STL::ARGB BackgroundColor = STL::ARGB(255, 60, 120, 180);

    void OpenAnimation(STL::Framebuffer* Buffer)
    {
        if (AnimationCounter > 60)
        {
            STL::System("set drawmouse 1");
            STL::System("start topbar");

            StartAnimation(nullptr);

            Buffer->Fill(BackgroundColor);
        }
        else
        {
            Buffer->Fill((BackgroundColor / (uint8_t)60) * (uint8_t)AnimationCounter);
        }
    }

    STL::PROR Procedure(STL::PROM Message, STL::PROI Input)
    {
        switch(Message)
        {
        case STL::PROM::INIT:
        {
            STL::PINFO* Info = (STL::PINFO*)Input;
            Info->Type = STL::PROT::GRANDFATHER;
            Info->Depth = 0;
            Info->Title = "Desktop";

            BackgroundColor = STL::ARGB(255, 60, 120, 180);

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
        default:
        {

        }
        break;
        }

        return STL::PROR::SUCCESS;
    }
}