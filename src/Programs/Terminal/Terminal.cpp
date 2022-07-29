#include "Terminal.h"

#include "STL/Graphics/Framebuffer.h"
#include "STL/String/String.h"
#include "STL/String/cstr.h"
#include "STL/System/System.h"

#include "Version.h"

#define NEWLINE_OFFSET RAISED_WIDTH * 3

namespace Terminal
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
            Info->Type = STL::PROT::CHILD;
            Info->Frame = STL::SimpleWindowFrame;
            Info->Depth = 1;
            Info->Left = 360;
            Info->Top = 200;
            Info->Width = 1000 + NEWLINE_OFFSET * 2;
            Info->Height = 650;
            Info->Title = "Terminal";

            Text = "";
            Command[0] = 0;
            CursorPos = STL::Point(0, 0);
            
            Write("\n\r");
            Write("Welcome to the Terminal of ");
            Write(OS_VERSION);
            Write("!\n\r");
            Write("Type \"help\" for a list of all available commands.\n\n\r");
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

            //Clear command
            for (uint32_t i = 0; i < STL::Length(Command) + 2; i++)
            {
                Buffer->PutChar(' ', STL::Point(CursorPos.X + 8 * i, CursorPos.Y), 1, STL::ARGB(255), STL::ARGB(0));
            }            

            //Scroll up
            if (CursorPos.Y + STL::LineAmount(Text.cstr()) * 16 + NEWLINE_OFFSET * 2 > Buffer->Height)
            {
                uint64_t Amount = (CursorPos.Y + STL::LineAmount(Text.cstr()) * 16 + NEWLINE_OFFSET * 2) - Buffer->Height;
                
                for (uint32_t y = NEWLINE_OFFSET; y < Buffer->Height - NEWLINE_OFFSET - Amount; y++)
                {
                    for (uint32_t x = NEWLINE_OFFSET; x < Buffer->Width - NEWLINE_OFFSET; x++)
                    {
                        *(STL::ARGB*)((uint64_t)Buffer->Base + x * 4 + y * Buffer->PixelsPerScanline * 4) = 
                        *(STL::ARGB*)((uint64_t)Buffer->Base + x * 4 + (y + Amount) * Buffer->PixelsPerScanline * 4);
                    }
                }

                for (uint32_t y = Buffer->Height - NEWLINE_OFFSET - Amount; y < Buffer->Height; y++)
                {
                    for (uint32_t x = NEWLINE_OFFSET; x < Buffer->Width - NEWLINE_OFFSET; x++)
                    {
                        *(STL::ARGB*)((uint64_t)Buffer->Base + x * 4 + y * Buffer->PixelsPerScanline * 4) = STL::ARGB(0);
                    }
                }

                CursorPos.Y -= Amount; 
                if (CursorPos.Y < 0)
                {
                    CursorPos.Y = 0;
                }
            }

            if (RedrawText)
            {               
                //Print text 
                Buffer->Print(Text.cstr(), CursorPos, 1, STL::ARGB(255), STL::ARGB(0), NEWLINE_OFFSET);
                Text = "";
                Command[0] = 0;
                RedrawText = false;

                //Draw Edge
                Buffer->DrawRect(STL::Point(0, 0), STL::Point(Buffer->Width, RAISED_WIDTH  * 2), STL::ARGB(200));
                Buffer->DrawRect(STL::Point(0, 0), STL::Point(RAISED_WIDTH * 2, Buffer->Height), STL::ARGB(200));
                Buffer->DrawRect(STL::Point(Buffer->Width - RAISED_WIDTH * 2, 0), STL::Point(Buffer->Width, Buffer->Height), STL::ARGB(200));
                Buffer->DrawRect(STL::Point(0, Buffer->Height - RAISED_WIDTH * 2), STL::Point(Buffer->Width, Buffer->Height), STL::ARGB(200));
                Buffer->DrawSunkenRectEdge(STL::Point(RAISED_WIDTH * 2, RAISED_WIDTH * 2), STL::Point(Buffer->Width - RAISED_WIDTH * 2, Buffer->Height - RAISED_WIDTH  * 2));
            }

            //Print command
            STL::Point Temp = CursorPos;
            Buffer->Print(Command, Temp, 1, STL::ARGB(255), STL::ARGB(0), NEWLINE_OFFSET);

            //Print underline
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
            Text = "";
            Command[0] = 0;
            CursorPos = STL::Point(0, 0);
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