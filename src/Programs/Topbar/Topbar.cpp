#include "Topbar.h"

#include "STL/Graphics/Framebuffer.h"
#include "STL/System/System.h"
#include "STL/GUI/Button.h"
#include "STL/GUI/Label.h"

#include "STL/String/cstr.h"

#include "Programs/Desktop/Desktop.h"

#define TOPBAR_PADDING 8 + RAISED_WIDTH * 4 
#define BUTTONGAP 125
#define BUTTONWIDTH 100


namespace Topbar
{                    
    STL::Label TimeDateLabel;

    STL::ARGB BackgroundColor;

    STL::Button SystemButton;
    STL::Button StartButton;

    void(*CurrentAnimation)(STL::Framebuffer*);
    uint64_t AnimationCounter = 0;

    bool SystemMenuStarted = false;
    bool StartMenuStarted = false;

    inline void StartAnimation(void(*Animation)(STL::Framebuffer*))
    {
        AnimationCounter = 0;
        CurrentAnimation = Animation;
    }

    void OpenAnimation(STL::Framebuffer* Buffer)
    {
        if (AnimationCounter == 0)
        {
            Buffer->DrawRect(STL::Point(0, 0), STL::Point(Buffer->Width, Buffer->Height), Desktop::BackgroundColor);
        }
        else if (AnimationCounter > Buffer->Height - RAISED_WIDTH)
        {
            StartAnimation(nullptr);   
            return;
        }

        uint64_t Offset = ((Buffer->Height - RAISED_WIDTH) - AnimationCounter);

        Buffer->DrawRaisedRect(STL::Point(RAISED_WIDTH, RAISED_WIDTH), STL::Point(Buffer->Width - RAISED_WIDTH, AnimationCounter), BackgroundColor);

        TimeDateLabel.TopLeft.Y = 4 + RAISED_WIDTH * 2 - Offset;
        TimeDateLabel.BottomRight.Y = Buffer->Height - 4 - RAISED_WIDTH * 2 - Offset;

        SystemButton.TopLeft.Y = 4 + RAISED_WIDTH * 2 - Offset;
        SystemButton.BottomRight.Y = Buffer->Height - 4 - RAISED_WIDTH * 2 - Offset;

        StartButton.TopLeft.Y = 4 + RAISED_WIDTH * 2 - Offset;
        StartButton.BottomRight.Y = Buffer->Height - 4 - RAISED_WIDTH * 2 - Offset;
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
            Info->Left = 0;
            Info->Top = 0;
            Info->Width = 1920;
            Info->Height = 16 + TOPBAR_PADDING + RAISED_WIDTH;
            Info->Title = "Topbar";

            BackgroundColor = STL::ARGB(255, 200, 200, 200);

            SystemMenuStarted = false;
            StartMenuStarted = false;

            TimeDateLabel = STL::Label("TEMP", STL::Point(Info->Width / 2 - 100, 4 + RAISED_WIDTH * 2), STL::Point(Info->Width / 2 + 100, Info->Height - 4 - RAISED_WIDTH * 2));
            TimeDateLabel.Style = STL::LabelStyle::Sunken;

            SystemButton = STL::Button(BackgroundColor, "System", STL::Point(Info->Width - BUTTONGAP - BUTTONWIDTH / 2, 4 + RAISED_WIDTH * 2), STL::Point(Info->Width - BUTTONGAP + BUTTONWIDTH / 2, Info->Height - 4 - RAISED_WIDTH * 2));
            StartButton = STL::Button(BackgroundColor, "Start", STL::Point(BUTTONGAP - BUTTONWIDTH / 2, 4 + RAISED_WIDTH * 2), STL::Point(BUTTONGAP + BUTTONWIDTH / 2, Info->Height - 4 - RAISED_WIDTH * 2));

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

            SystemButton.Draw(Buffer);
            StartButton.Draw(Buffer);
            TimeDateLabel.Draw(Buffer);
        }
        break;
        case STL::PROM::TICK:
        {
            uint64_t CurrentTick = *(uint64_t*)Input;

            if (CurrentAnimation != nullptr || CurrentTick % 100 == 0)
            {
                TimeDateLabel.Text = STL::System("time");
                TimeDateLabel.Text += " ";
                TimeDateLabel.Text += STL::System("date");

                return STL::PROR::DRAW;
            }
        }
        break;
        case STL::PROM::MOUSE:
        {
            STL::MDATA MouseInfo = *(STL::MDATA*)Input;

            if (SystemButton.IsToggled(MouseInfo))
            {
                if (SystemMenuStarted)
                {
                    STL::System("kill SystemMenu");
                }
                else
                {
                    STL::System("start SystemMenu");
                }
                SystemMenuStarted = !SystemMenuStarted;
            }
            
            if (StartButton.IsToggled(MouseInfo))
            {
                if (StartMenuStarted)
                {
                    STL::System("kill StartMenu");
                }
                else
                {
                    STL::System("start StartMenu");
                }
                StartMenuStarted = !StartMenuStarted;
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