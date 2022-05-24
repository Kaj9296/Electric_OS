#include "Calculator.h"

#include "STL/Graphics/Framebuffer.h"
#include "STL/String/String.h"
#include "STL/String/cstr.h"
#include "STL/System/System.h"
#include "STL/GUI/Button.h"
#include "STL/GUI/Label.h"

#define BUTTON_SIZE STL::Point(50, 50 / (16/9))

#define LABEL_POS STL::Point(RAISED_WIDTH * 2, RAISED_WIDTH * 2)
#define LABEL_SIZE STL::Point(RAISED_WIDTH * 2 + 40 + 3 * (RAISED_WIDTH * 3 + BUTTON_SIZE.X), 40)

namespace Calculator
{
    STL::Label Label;

    STL::Button NumButtons[10];

    STL::Button AddButton;
    STL::Button SubButton;
    STL::Button MulButton;
    STL::Button DivButton;

    STL::Button EqualButton;
    STL::Button PointButton;

    uint32_t PreviousNum = 0;
    uint32_t PreviousOperator = 0;

    bool ClearLabel = false;

    STL::Point GetButtonPosition(uint8_t X, uint8_t Y)
    {
        return STL::Point(RAISED_WIDTH * 2 + X * (RAISED_WIDTH * 3 + BUTTON_SIZE.X), RAISED_WIDTH * 5 + LABEL_SIZE.Y + Y * (RAISED_WIDTH * 3 + BUTTON_SIZE.X));
    }

    STL::PROR Procedure(STL::PROM Message, STL::PROI Input)
    {
        switch(Message)
        {
        case STL::PROM::INIT:
        {
            STL::PINFO* Info = (STL::PINFO*)Input;
            Info->Type = STL::PROT::WINDOWED;
            Info->Depth = 1;
            Info->Left = 360;
            Info->Top = 200;
            Info->Width = LABEL_POS.X + LABEL_SIZE.X + RAISED_WIDTH * 3;
            Info->Height = RAISED_WIDTH * 4 + LABEL_SIZE.Y + 4 * (RAISED_WIDTH * 3 + BUTTON_SIZE.X);
            Info->Title = "Calculator";

            PreviousNum = 0;
            ClearLabel = true;
            PreviousOperator = 0;

            Label = STL::Label("0", LABEL_POS, LABEL_POS + LABEL_SIZE);
            Label.Style = STL::LabelStyle::Sunken;
            Label.Background = STL::ARGB(255);
            Label.Foreground = STL::ARGB(0);
            Label.Scale = 2;
            Label.HorizontalAlign = STL::LabelAlign::Positive;

            STL::Point Temp = GetButtonPosition(3, 2);
            AddButton = STL::Button(STL::ARGB(200), "+", Temp, Temp + BUTTON_SIZE);
            AddButton.Scale = 2;

            Temp = GetButtonPosition(3, 1);
            SubButton = STL::Button(STL::ARGB(200), "-", Temp, Temp + BUTTON_SIZE);
            SubButton.Scale = 2;

            Temp = GetButtonPosition(3, 0);
            MulButton = STL::Button(STL::ARGB(200), "*", Temp, Temp + BUTTON_SIZE);
            MulButton.Scale = 2;

            Temp = GetButtonPosition(0, 3);
            DivButton = STL::Button(STL::ARGB(200), "/", Temp, Temp + BUTTON_SIZE);
            DivButton.Scale = 2;

            Temp = GetButtonPosition(3, 3);
            EqualButton = STL::Button(STL::ARGB(200), "=", Temp, Temp + BUTTON_SIZE);
            EqualButton.Scale = 2;

            Temp = GetButtonPosition(2, 3);
            PointButton = STL::Button(STL::ARGB(200), ".", Temp, Temp + BUTTON_SIZE);
            PointButton.Scale = 2;

            Temp = GetButtonPosition(1, 3);
            NumButtons[0] = STL::Button(STL::ARGB(200), "0", Temp, Temp + BUTTON_SIZE);
            NumButtons[0].Scale = 2;

            Temp = GetButtonPosition(0, 0);
            NumButtons[1] = STL::Button(STL::ARGB(200), "1", Temp, Temp + BUTTON_SIZE);
            NumButtons[1].Scale = 2;
            
            Temp = GetButtonPosition(1, 0);
            NumButtons[2] = STL::Button(STL::ARGB(200), "2", Temp, Temp + BUTTON_SIZE);
            NumButtons[2].Scale = 2;
            
            Temp = GetButtonPosition(2, 0);
            NumButtons[3] = STL::Button(STL::ARGB(200), "3", Temp, Temp + BUTTON_SIZE);
            NumButtons[3].Scale = 2;
            
            Temp = GetButtonPosition(0, 1);
            NumButtons[4] = STL::Button(STL::ARGB(200), "4", Temp, Temp + BUTTON_SIZE);
            NumButtons[4].Scale = 2;
            
            Temp = GetButtonPosition(1, 1);
            NumButtons[5] = STL::Button(STL::ARGB(200), "5", Temp, Temp + BUTTON_SIZE);
            NumButtons[5].Scale = 2;
            
            Temp = GetButtonPosition(2, 1);
            NumButtons[6] = STL::Button(STL::ARGB(200), "6", Temp, Temp + BUTTON_SIZE);
            NumButtons[6].Scale = 2;
            
            Temp = GetButtonPosition(0, 2);
            NumButtons[7] = STL::Button(STL::ARGB(200), "7", Temp, Temp + BUTTON_SIZE);
            NumButtons[7].Scale = 2;
            
            Temp = GetButtonPosition(1, 2);
            NumButtons[8] = STL::Button(STL::ARGB(200), "8", Temp, Temp + BUTTON_SIZE);
            NumButtons[8].Scale = 2;
            
            Temp = GetButtonPosition(2, 2);
            NumButtons[9] = STL::Button(STL::ARGB(200), "9", Temp, Temp + BUTTON_SIZE);
            NumButtons[9].Scale = 2;
        }
        break;
        case STL::PROM::DRAW:
        {
            STL::Framebuffer* Buffer = (STL::Framebuffer*)Input;

            Buffer->DrawRect(STL::Point(0, 0), STL::Point(Buffer->Width, Buffer->Height), STL::ARGB(200));

            Label.Draw(Buffer);

            AddButton.Draw(Buffer);
            SubButton.Draw(Buffer);
            MulButton.Draw(Buffer);
            DivButton.Draw(Buffer);

            EqualButton.Draw(Buffer);
            PointButton.Draw(Buffer);

            for (int i = 0; i < 10; i++)
            {
                NumButtons[i].Draw(Buffer);
            }
        }
        break;
        case STL::PROM::KEYPRESS:
        {
            /*uint8_t Key = *(uint8_t*)Input;

            if (Key >= '0' && Key <= '9')
            {
                if (ClearLabel)
                {
                    Label.Text = Key;
                    ClearLabel = false; 
                }
                else if (Label.Text.Length() < LABEL_SIZE.X / 16 - 1)
                {
                    Label.Text += Key;     
                }
            }*/
            
            return STL::PROR::DRAW;
        }
        break;        
        case STL::PROM::MOUSE:
        {
            STL::MDATA MouseInfo = *(STL::MDATA*)Input;

            for (int i = 0; i < 10; i++)
            {
                if (NumButtons[i].IsPressed(MouseInfo))
                {
                    if (ClearLabel)
                    {
                        Label.Text = NumButtons[i].Text;
                        ClearLabel = false; 
                    }
                    else if (Label.Text.Length() < (uint32_t)LABEL_SIZE.X / 16 - 1)
                    {
                        Label.Text += NumButtons[i].Text;     
                    }
                }
            }

            if (EqualButton.IsPressed(MouseInfo))
            {
                if (PreviousOperator == '+')
                {
                    uint64_t NewNum = PreviousNum + STL::ToInt(Label.Text.cstr());
                    Label.Text = STL::ToString(NewNum);     
                    PreviousNum = 0;   
                }
                else if (PreviousOperator == '-')
                {
                    uint64_t NewNum = PreviousNum - STL::ToInt(Label.Text.cstr());
                    Label.Text = STL::ToString(NewNum);     
                    PreviousNum = 0;               
                }
                else if (PreviousOperator == '*')
                {
                    uint64_t NewNum = PreviousNum * STL::ToInt(Label.Text.cstr());
                    Label.Text = STL::ToString(NewNum); 
                    PreviousNum = 0;               
                }
                else if (PreviousOperator == '/')
                {
                    uint64_t Devisor = STL::ToInt(Label.Text.cstr());
                    if (Devisor != 0)
                    {
                        uint64_t NewNum = PreviousNum / Devisor;
                        Label.Text = STL::ToString(NewNum);                         
                    }
                    else 
                    {
                        Label.Text = "DIV BY ZERO!";
                    }
                    PreviousNum = 0;               
                }

                ClearLabel = true;   
                PreviousOperator = 0;
            }

            if (PointButton.IsPressed(MouseInfo))
            {
                
            }

            if (AddButton.IsPressed(MouseInfo))
            {
                uint64_t NewNum = PreviousNum + STL::ToInt(Label.Text.cstr());
                Label.Text = STL::ToString(NewNum);
                ClearLabel = true;
                PreviousOperator = '+';
                PreviousNum = NewNum;
            }

            if (SubButton.IsPressed(MouseInfo))
            {
                uint64_t NewNum = PreviousNum - STL::ToInt(Label.Text.cstr());
                Label.Text = STL::ToString(NewNum);
                ClearLabel = true;
                PreviousOperator = '-';
                PreviousNum = NewNum;
            }

            if (MulButton.IsPressed(MouseInfo))
            {
                uint64_t NewNum = STL::ToInt(Label.Text.cstr());
                Label.Text = STL::ToString(PreviousNum * NewNum);
                ClearLabel = true;
                PreviousOperator = '*';
                PreviousNum = NewNum;              
            }

            if (DivButton.IsPressed(MouseInfo))
            {
                uint64_t NewNum = STL::ToInt(Label.Text.cstr());
                if (NewNum != 0)
                {
                    Label.Text = STL::ToString(PreviousNum / NewNum);
                }
                else 
                {
                    Label.Text = "DIV BY ZERO!";
                }
                ClearLabel = true;
                PreviousOperator = '/';
                PreviousNum = NewNum;
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