#include "tty.h"

#include "../Rendering/Renderer.h"
#include "../UserInput/KeyBoard.h"
#include "../PIT/PIT.h"
#include "../String/cstr.h"
#include "../System/System.h"

namespace tty
{
    uint8_t Scale; 

    ARGB Background;
    ARGB Foreground;

    char Input[128];

    void Print(const char* Message)
    {
        Renderer::Print(Message, Background, Foreground, Scale);
    }

    void RunCommand(const char* Command)
    {
        if (cstr::HashWord(Command) == cstr::HashWord("clear"))
        {
            Renderer::Clear(ARGB(0));
        }
        else
        {
            char* Output = (char*)System::System(Command);
            while (*Output != 0)
            {
                if (*Output == '\033')
                {
                    Output++;
                    switch (*Output)
                    {
                    case 'F':
                    {
                        Foreground = ARGB(255, (Output[1] - '0') * 100 + (Output[2] - '0') * 10 + (Output[3] - '0'), 
                        (Output[4] - '0') * 100 + (Output[5] - '0') * 10 + (Output[6] - '0'), 
                        (Output[7] - '0') * 100 + (Output[8] - '0') * 10 + (Output[9] - '0'));
                    }
                    break;
                    case 'B':
                    {
                        Background = ARGB(255, (Output[1] - '0') * 100 + (Output[2] - '0') * 10 + (Output[3] - '0'), 
                        (Output[4] - '0') * 100 + (Output[5] - '0') * 10 + (Output[6] - '0'), 
                        (Output[7] - '0') * 100 + (Output[8] - '0') * 10 + (Output[9] - '0'));
                    }
                    break;
                    }
                    Output += 9;
                }
                else
                {
                    Renderer::Print(*Output, Background, Foreground, Scale);
                }
                Output++;
            }
        }
    }

    void tty()
    {
        Scale = 1;
        Background = ARGB(0);
        Foreground = ARGB(255);

        Renderer::Clear(Background);

        for (int i = 0; i < sizeof(Input)/sizeof(Input[0]); i++)
        {
            Input[i] = 0;
        }

        Print("\n\r");
        RunCommand("sysfetch");
        Print("\n\r");

        while (true)
        {
            Print("> ");

            Point StartCursorPos = Renderer::CursorPos;
            uint64_t PrevTick = PIT::Ticks;
            bool Alternate = false;
            int i = 0;
            while(true)
            {
                uint8_t Key = KeyBoard::GetKeyPress();
                if (Key != 0)
                {
                    if (Key == ENTER || i >= sizeof(Input)/sizeof(Input[0]) - 1)
                    {   
                        Renderer::CursorPos = StartCursorPos;
                        for (int j = 0; j <= i; j++)
                        {
                            Print("  ");
                        }
                        Renderer::CursorPos = StartCursorPos;
                        Print(Input);
                        break;
                    }
                    else if (Key == BACKSPACE && i != 0)
                    {
                        Input[i] = 0;
                        Input[i + 1] = 0;
                        Renderer::CursorPos.X -= 8 * Scale;
                        Print("  ");
                        Renderer::CursorPos.X -= 16 * Scale;
                        i--;
                    }
                    else
                    {
                        Input[i] = Key;
                        Input[i + 1] = 0;
                        Renderer::CursorPos = StartCursorPos;
                        for (int j = 0; j <= i; j++)
                        {
                            Print("  ");
                        }
                        Renderer::CursorPos = StartCursorPos;
                        Print(Input);
                        i++;
                    }
                }
                else if (PrevTick + PIT::GetFrequency() / 2 < PIT::Ticks)
		        {
                    Alternate = !Alternate;
                    PrevTick = PIT::Ticks;

                    if (Alternate)
                    {
                        Renderer::PutChar('_', Background, Foreground, Renderer::CursorPos, Scale);
                    }
                    else
                    {
                        Renderer::PutChar(' ', Background, Foreground, Renderer::CursorPos, Scale);
                    }
                }

                asm("HLT");
            }

            Print("\n\r");
            RunCommand(Input);
            Input[0] = 0;
            Print("\n\r");
        }   
    }
}