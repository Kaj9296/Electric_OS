#include "System.h"

#include "../String/cstr.h"
#include "../UserInput/Mouse.h"
#include "../RTC/RTC.h"
#include "../PIT/PIT.h"
#include "../Debug/Debug.h"
#include "../Memory/Paging/PageAllocator.h"

namespace System
{
    struct Command
    {
        const char* Name;
        uint64_t Hash;
        const char* (*Function)(const char*);

        Command(const char* Name, const char* (*Function)(const char*))
        {
            this->Function = Function;
            this->Name = Name;
            this->Hash = cstr::HashWord(Name);
        }
    };

    struct SettableInt
    {
        const char* Name;
        uint64_t Hash;

        void* Variable;
        uint8_t VariableSize;

        void SetVariable(uint64_t Value)
        {
            switch(VariableSize)
            {
            case 1:
            {
                *((uint8_t*)Variable) = Value;
            }
            break;
            case 2:
            {
                *((uint16_t*)Variable) = Value;
            }
            break;
            case 4:
            {
                *((uint32_t*)Variable) = Value;
            }
            break;
            case 8:
            {
                *((uint64_t*)Variable) = Value;
            }
            break;
            }
        }

        SettableInt(const char* Name, void* Variable, uint8_t VariableSize)
        {
            this->Name = Name;
            this->Hash = cstr::HashWord(Name);
            this->Variable = Variable;
            this->VariableSize = VariableSize;
        }
    };

    const char* CommandSet(const char* Command)
    {
        SettableInt SettableInts[] =
        {
            SettableInt("mousedraw", &Mouse::Draw, sizeof(Mouse::Draw))
        };

        char* Variable = cstr::NextWord(Command);
        char* Value = cstr::NextWord(Variable);

        uint64_t VariableHash = cstr::HashWord(Variable);

        for (int i = 0; i < sizeof(SettableInts)/sizeof(SettableInts[0]); i++)
        {
            if (VariableHash == SettableInts[i].Hash)
            {
                SettableInts[i].SetVariable(cstr::ToInt(Value));
                return "Variable set";
            }
        }

        return "ERROR: Variable not found";
    }
    
    const char* CommandHelp(const char* Command)
    {       
        return 
        FOREGROUND_COLOR(086, 182, 194)"set [VARIABLE] [VALUE]\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    DESC:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        Sets the specified kernel variable to the specified value\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    VARIABLE:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        mousedraw\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    VALUE:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        Any positive integer\n\r"
        FOREGROUND_COLOR(086, 182, 194)"help\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    DESC:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        Prints this menu\n\r"
        FOREGROUND_COLOR(086, 182, 194)"time\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    DESC:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        Prints the current time\n\r"
        FOREGROUND_COLOR(086, 182, 194)"date\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    DESC:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        Prints the current date\n\r"
        FOREGROUND_COLOR(086, 182, 194)"panic\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    DESC:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        Causes a kernel panic\n\r"
        FOREGROUND_COLOR(086, 182, 194)"sysfetch\n\r"
        FOREGROUND_COLOR(224, 108, 117)"    DESC:\n\r"
        FOREGROUND_COLOR(255, 255, 255)"        A neofetch lookalike to give system information\n\r";
    }

    char CommandTimeOutput[64];
    const char* CommandTime(const char* Command)
    {
        char* Temp = CommandTimeOutput;
        Temp = cstr::Copy(Temp, cstr::ToString(RTC::GetHour())) + 1;
        Temp = cstr::Copy(Temp, ":") + 1;
        Temp = cstr::Copy(Temp, cstr::ToString(RTC::GetMinute())) + 1;
        Temp = cstr::Copy(Temp, ":") + 1;
        Temp = cstr::Copy(Temp, cstr::ToString(RTC::GetSecond())) + 1;
        *Temp = 0;
        return CommandTimeOutput;
    }

    char CommandDateOutput[64];
    const char* CommandDate(const char* Command)
    {
        char* Temp = CommandDateOutput;
        Temp = cstr::Copy(Temp, cstr::ToString(RTC::GetDay())) + 1;
        Temp = cstr::Copy(Temp, "/") + 1;
        Temp = cstr::Copy(Temp, cstr::ToString(RTC::GetMonth())) + 1;
        Temp = cstr::Copy(Temp, "/") + 1;
        Temp = cstr::Copy(Temp, "20") + 1;
        Temp = cstr::Copy(Temp, cstr::ToString(RTC::GetYear())) + 1;
        *Temp = 0;
        return CommandDateOutput;
    }

    const char* CommandPanic(const char* Command)
    {
        Debug::Error("Test!");
        return nullptr;
    }

    char CommandSysfetchOutput[1100];
    const char* CommandSysfetch(const char* Command)
    {                
        char* Temp = CommandSysfetchOutput;          
     
        auto WriteLine = [&](const char* Part1, const char* Part2 = nullptr) 
        { 
            Temp = cstr::Copy(Temp, FOREGROUND_COLOR(086, 182, 194)) + 1;
            Temp = cstr::Copy(Temp, Part1) + 1;
            Temp = cstr::Copy(Temp, FOREGROUND_COLOR(255, 255, 255)) + 1;
            if (Part2 != nullptr)
            {
                Temp = cstr::Copy(Temp, Part2) + 1;
            }
        };  

        auto Write = [&](const char* String)
        {
            Temp = cstr::Copy(Temp, String) + 1;
        };

        WriteLine("        /ooooooooooo/   ", nullptr);
        Write("\n\r");

        WriteLine("       /ooooooooooo/     OS", ": ");
        Write(OS_VERSION);
        Write("\n\r");  

        WriteLine("      /ooooooooooo/      Uptime", ": ");
        Write(cstr::ToString(PIT::Ticks / PIT::GetFrequency()));
        Write(" s");
        Write("\n\r"); 

        WriteLine("     /oooooooooo/        Time", ": ");
        Write(CommandTime(nullptr));  
        Write("\n\r"); 

        WriteLine("    /oooooooooo/         Date", ": ");  
        Write(CommandDate(nullptr));  
        Write("\n\r");    

        WriteLine("   /oooooooooo/          Memory", ": ");  
        Write(cstr::ToString(PageAllocator::GetLockedMemory() / 1048576));
        Write(" / ");
        Write(cstr::ToString((PageAllocator::GetFreeMemory() + PageAllocator::GetLockedMemory()) / 1048576));
        Write(" MB"); 
        Write("\n\r");  

        WriteLine("  /oooooooooo/          ", nullptr); 
        Write("\n\r");   

        WriteLine(" /ooooooooooooooooooooo/", nullptr);   
        Write("\n\r");    

        WriteLine("/ooooooooooooooooooooo/ ", nullptr); 
        Write("\n\r");    

        WriteLine("/oooooooooooooooooooo/  ", nullptr); 
        Write("\n\r");   

        WriteLine("oooooooooooooooooooo/    ", nullptr); 
        Write("\n\r");   

        WriteLine("          /ooooooo/     ", nullptr);  
        Write("\n\r");   

        WriteLine("         /oooooo/       ", nullptr);  
        Write("\n\r"); 

        WriteLine("        /oooooo/        ", nullptr);  
        Write("\n\r");  

        WriteLine("        /oooo/          ", nullptr);  
        Write(" \033B040044052   \033B224108117   \033B229192123   \033B152195121   \033B097175239   \033B198120221   \033B086182194   \033B220223228   ");  
        Write("\033B000000000"); 
        Write("\n\r");   

        WriteLine("       /oooo/           ", nullptr);   
        Write(" \033B040044052   \033B224108117   \033B229192123   \033B152195121   \033B097175239   \033B198120221   \033B086182194   \033B220223228   ");   
        Write("\033B000000000"); 
        Write("\n\r");

        WriteLine("      /oooo/            ", nullptr);   
        Write("\n\r");

        WriteLine("     /ooo/              ", nullptr);
        Write("\n\r");

        *Temp = 0;                   
        return CommandSysfetchOutput;
    }                                                                                                                                                            

    const char* System(const char* Input)
    {    
        Command Commands[] =
        {
            Command("set", CommandSet),
            Command("help", CommandHelp),
            Command("time", CommandTime),
            Command("date", CommandDate),
            Command("panic", CommandPanic),
            Command("sysfetch", CommandSysfetch)
        };

        uint64_t Hash = cstr::HashWord(Input);
        for (int i = 0; i < sizeof(Commands)/sizeof(Commands[0]); i++)
        {
            if (Hash == Commands[i].Hash)
            {
                return Commands[i].Function(Input);
            }
        }

        return "ERROR: Command not found";
    }

    /*const char* System(const char* Input)
    {
        return Input;
    }*/
}