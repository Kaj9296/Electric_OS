#include "tty.h"

#include "STL/Graphics/Framebuffer.h"
#include "STL/String/String.h"
#include "STL/String/cstr.h"
#include "STL/System/System.h"

#include "Version.h"

namespace tty
{
    uint64_t PrevTick = 0;

    char Command[64];
    STL::String Text;
    STL::Point CursorPos = STL::Point(0, 0);

    bool RedrawText = false;
    bool DrawUnderline = false;
    bool ClearCommand = false;

    void Write(const char* cstr)
    {
        Text += cstr;
    }

    STL::PROR Procedure(STL::PROM Message, STL::PROI Input)
    {
        switch(Message)
        {
        case STL::PROM::INIT:
        {
            STL::PINFO* Info = (STL::PINFO*)Input;
            Info->Type = STL::PROT::GRANDFATHER;
            Info->Title = "tty";
            Info->Depth = 0;

            Text = "";
            Command[0] = 0;
            CursorPos = STL::Point(0, 0);
            
            Write("\n\r");
            Write("Welcome to the tty of ");
            Write(OS_VERSION);
            Write("!\n\r");
            Write("Type \"help\" or \"start desktop\" to get started.\n\n\r");
            Write(STL::System("sysfetch"));
            Write("\n\r");
            Write("> ");

            RedrawText = true;
        }
        break;
        case STL::PROM::TICK:
        {   
            uint64_t CurrentTick = *(uint64_t*)Input;

            if (PrevTick + 50 < CurrentTick)
            {
                DrawUnderline = !DrawUnderline;
                PrevTick = CurrentTick;

                return STL::PROR::DRAW;
            }
        }
        break;
        case STL::PROM::DRAW:
        {
            STL::Framebuffer* Buffer = (STL::Framebuffer*)Input;

            if (RedrawText)
            {                
                for (uint32_t i = 0; i < STL::Length(Command) + 2; i++)
                {
                    Buffer->PutChar(' ', STL::Point(CursorPos.X + 8 * i, CursorPos.Y), 1, STL::ARGB(255), STL::ARGB(0));
                }

                if (CursorPos.Y + STL::LineAmount(Text.cstr()) * 16 > Buffer->Height)
                {
                    uint64_t Amount = (CursorPos.Y + STL::LineAmount(Text.cstr()) * 16) - Buffer->Height;
                    Buffer->ScrollUp(Amount);
                    CursorPos.Y -= Amount; 
                    if (CursorPos.Y < 0)
                    {
                        CursorPos.Y = 0;
                    }
                }

                Buffer->Print(Text.cstr(), CursorPos, 1, STL::ARGB(255), STL::ARGB(0));
                Text = "";
                Command[0] = 0;
                RedrawText = false;
            }

            if (ClearCommand)
            {
                for (uint32_t i = 0; i < STL::Length(Command) + 2; i++)
                {
                    Buffer->PutChar(' ', STL::Point(CursorPos.X + 8 * i, CursorPos.Y), 1, STL::ARGB(255), STL::ARGB(0));
                }
                ClearCommand = false;
            }
            
            STL::Point Temp = CursorPos;
            Buffer->Print(Command, Temp, 1, STL::ARGB(255), STL::ARGB(0));

            if (DrawUnderline)
            {
                Buffer->PutChar('_', Temp, 1, STL::ARGB(255), STL::ARGB(0));
            }
            else
            {
                Buffer->PutChar(' ', Temp, 1, STL::ARGB(255), STL::ARGB(0));
            }
        }
        break;
        case STL::PROM::KEYPRESS:
        {
            uint8_t Key = *(uint8_t*)Input;

            if (Key == ENTER)
            {   
                Text += Command;
                Text += "\n\r";
                Text += STL::System(Command);
                Text += "\n\r> ";
                RedrawText = true;
            }
            else if (Key == BACKSPACE)
            {
                uint64_t CommandLength = STL::Length(Command);

                if (CommandLength != 0)
                {
                    Command[CommandLength - 1] = 0;
                    Command[CommandLength] = 0;
                    ClearCommand = true;
                }
            }
            else
            {               
                uint64_t CommandLength = STL::Length(Command);
                if (CommandLength < 63)
                {
                    Command[CommandLength] = Key;
                    Command[CommandLength + 1] = 0;
                }
            }

            return STL::PROR::DRAW;
        }
        break;
        case STL::PROM::CLEAR:
        {
            Text = "> ";
            Command[0] = 0;
            CursorPos = STL::Point(0, 0);
            RedrawText = true;

            return STL::PROR::DRAW;
        }
        break;
        case STL::PROM::KILL:
        {
            Text = "";
            Command[0] = 0;
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