#include "System.h"

#include "STL/String/cstr.h"
#include "STL/Process/Process.h"
#include "STL/Graphics/Graphics.h"

#include "Programs/tty/tty.h"
#include "Programs/Desktop/Desktop.h"
#include "Programs/Topbar/Topbar.h"
#include "Programs/SystemMenu/SystemMenu.h"
#include "Programs/StartMenu/StartMenu.h"
#include "Programs/Terminal/Terminal.h"
#include "Programs/Calculator/Calculator.h"
#include "Programs/Tetris/Tetris.h"

#include "Kernel/Renderer/Renderer.h"
#include "Kernel/RTC/RTC.h"
#include "Kernel/PIT/PIT.h"
#include "Kernel/Debug/Debug.h"
#include "Kernel/IO/IO.h"
#include "Kernel/Memory/Paging/PageAllocator.h"
#include "Kernel/Memory/Heap.h"
#include "Kernel/ProcessHandler/ProcessHandler.h"
#include "Kernel/ProcessHandler/Compositor.h"
#include "Kernel/ACPI/ACPI.h"
#include "Kernel/PCI/PCI.h"
#include "Kernel/UEFI/UEFI.h"
#include "Kernel/AHCI/AHCI.h"

#include "Version.h"

#include <cstdarg>

namespace System
{
    static char CommandOutput[1024];

    const char* CommandSet(const char* Command)
    {        
        const char* Variable = STL::NextWord(Command);
        const char* Value = STL::NextWord(Variable);

        struct SettableVar
        {
            const char* Name;
            uint64_t Hash;
            void* Ptr;
            uint64_t Size;

            SettableVar(const char* Name, void* Ptr, uint64_t Size)
            {
                this->Name = Name;
                this->Hash = STL::HashWord(Name);
                this->Ptr = Ptr;
                this->Size = Size;
            }
        };

        SettableVar SettableVars[] =
        {
            SettableVar("drawmouse", &Compositor::DrawMouse, sizeof(Compositor::DrawMouse)),
        };

        uint64_t Hash = STL::HashWord(Variable);
        for (uint32_t i = 0; i < sizeof(SettableVars)/sizeof(SettableVars[0]); i++)
        {
            if (Hash == SettableVars[i].Hash)
            {
                switch (SettableVars[i].Size)
                {
                case 1:
                {
                    *(uint8_t*)SettableVars[i].Ptr = STL::ToInt(Value);
                }   
                break;
                case 2:
                {
                    *(uint16_t*)SettableVars[i].Ptr = STL::ToInt(Value);
                }   
                break;
                case 4:
                {
                    *(uint32_t*)SettableVars[i].Ptr = STL::ToInt(Value);
                }   
                break;
                case 8:
                {
                    *(uint64_t*)SettableVars[i].Ptr = STL::ToInt(Value);
                }   
                break;
                }
                
                return "Variable set";
            }
        }

        return "ERROR: Variable not found";
    }

    const char* CommandList(const char* Command)
    {         
        char* CurrentLocation = CommandOutput;
        char* LineStart = CommandOutput;

        auto Write = [&](const char* Input) 
        { 
            CurrentLocation = STL::CopyString(CurrentLocation, Input) + 1;
        }; 

        auto NextEntry = [&](const char* Input) 
        { 
            uint32_t Amount = 33 - ((uint64_t)CurrentLocation - (uint64_t)LineStart) % 33;
            for (uint32_t i = 0; i < Amount; i++)
            {
                Write(" ");            
            }    
            CurrentLocation = STL::CopyString(CurrentLocation, "| ") + 1;
            Write(Input);
        }; 

        auto StartLine = [&](const char* Input) 
        { 
            Write("| ");
            Write(Input);
        }; 

        auto NewLine = [&]() 
        {
            *CurrentLocation = '\n';
            CurrentLocation++;            
            *CurrentLocation = '\r';
            CurrentLocation++;
            LineStart = CurrentLocation;      
        };

        auto EndLine = [&](const char* Input) 
        { 
            NextEntry(Input);
            NextEntry("");            
            NewLine();
        };
            
        auto WriteLine = [&](uint64_t EntryAmount) 
        {            
            for (uint32_t i = 0; i < EntryAmount; i++)
            {
                CurrentLocation = STL::CopyString(CurrentLocation, "+--------------------------------") + 1;                
            }            
            *CurrentLocation = '+';
            CurrentLocation++;             
            NewLine();
        };

        uint64_t Hash = STL::HashWord(STL::NextWord(Command));
        switch (Hash)
        {
        case STL::ConstHashWord("process"):
        {                  
            WriteLine(2);
      
            StartLine("TITLE");
            EndLine("ID");

            WriteLine(2);

            /*for (uint32_t i = 0; i < ProcessHandler::Processes.Length(); i++)
            {                
                StartLine(ProcessHandler::Processes[i]->GetTitle());

                EndLine(STL::ToString(ProcessHandler::Processes[i]->GetID()));
            }*/

            WriteLine(2);
        }
        break;
        case STL::ConstHashWord("pci"):
        {                        
            WriteLine(2);
             
            StartLine("VENDOR");
            EndLine("TYPE");

            WriteLine(2);
                    
            DeviceHeader* Device;
            PCI::ResetEnumeration();
            while (PCI::Enumerate(Device))
            {                
                StartLine(Device->GetVendorString());

                EndLine(Device->GetTypeString());
            }  

            WriteLine(2);  
        }
        break;
        case STL::ConstHashWord("sata"):
        {                    
            WriteLine(2);   

            StartLine("PORT");
            EndLine("TYPE");

            WriteLine(2);
                    
            for (int i = 0; i < 32; i++)
            {                                
                StartLine(STL::ToString(i));

                if ((AHCI::GetABAR()->PortsImplemented) & (1 << i))
                {
                    switch (AHCI::GetABAR()->Ports[i].GetPortType())
                    {
                    case HBAPortType::SATAPI:
                    {                
                        EndLine("SATAPI");
                    }
                    break;
                    case HBAPortType::SATA:
                    {                
                        EndLine("SATA");
                    }
                    break;
                    case HBAPortType::PM:
                    {                
                        EndLine("PM");
                    }
                    break;
                    case HBAPortType::SEMB:
                    {                
                        EndLine("SEMB");
                    }
                    break;
                    default:
                    {                
                        EndLine("NONE");
                    }
                    break;
                    }
                }
                else
                {
                    EndLine("NOT IMPLEMENTED");
                }
            }  

            WriteLine(2);  
        }
        break;
        default:
        {
            return "ERROR: List not found";
        }
        break;
        }

        *(CurrentLocation - 2) = 0;
        return CommandOutput;
    }

    const char* CommandHelp(const char* Command)
    {              
        struct Manual
        {
            const char* Name;
            uint64_t Hash;
            const char* String;
            const char* Description;

            Manual(const char* Name, const char* Description, const char* String)
            {
                this->Name = Name;
                this->Hash = STL::HashWord(Name);
                this->String = String;
                this->Description = Description;
            }
        }; 

        Manual Manuals[] =
        {
            Manual("set", "An interface to access kernel variables.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    set - An interface to access kernel variables.\n\n\r"
            FOREGROUND_COLOR(086, 182, 194)"SYNOPSIS:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    set [VARIABLE] [VALUE]\n\n\r"
            FOREGROUND_COLOR(224, 108, 117)"    VARIABLE:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        drawmouse - A boolean value that sets if a cursor is drawn to the screen.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        font - A byte value that sets what font is used to render text.\n\n\r"
            FOREGROUND_COLOR(086, 182, 194)"    VALUE:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        Any positive integer.\n\n\r"
            ),
            Manual("list", "An interface to list information about the kernel or hardware.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    list - An interface to list information about the kernel or hardware.\n\n\r"
            FOREGROUND_COLOR(086, 182, 194)"SYNOPSIS:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    list [LIST]\n\n\r"
            FOREGROUND_COLOR(224, 108, 117)"    LIST:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        process - A list of all currently running processes.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        pci - A list of all connected PCI devices.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        sata - A list of all sata ports.\n\r"
            ),
            Manual("time", "Allows access to the time values read from the appropriate CMOS registers.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    time - Allows access to the time values read from the appropriate CMOS registers.\n\r"
            ),
            Manual("date", "Allows access to the time values read from the appropriate CMOS registers.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    date - Allows access to the time values read from the appropriate CMOS registers.\n\r"
            ),           
            Manual("clear", "Send a request to clear the framebuffer of the process that called the command.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    clear - Send a request to clear the framebuffer of the process that called the command.\n\r"
            ),/*           
            Manual("setfont", "Changes the font used to render text to the screen.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    setfont - Changes the font used to render text to the screen.\n\r"
            FOREGROUND_COLOR(086, 182, 194)"SYNOPSIS:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    setfont [FONT]\n\n\r"
            FOREGROUND_COLOR(224, 108, 117)"    FONT:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        zap-vga16\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        zap-light16\n\r"
            ),*/            
            Manual("start", "An interface to start any kernel level processes.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    start - An interface to start any kernel level processes.\n\n\r"
            FOREGROUND_COLOR(086, 182, 194)"SYNOPSIS:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    start [PROCESS]\n\n\r"
            FOREGROUND_COLOR(224, 108, 117)"    PROCESS:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        calculator - A calculator.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        desktop - The process responsible for drawing the desktop background.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        topbar - The process responsible for giving the user access to the startmenu and systemmenu.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        startmenu - The process used to start process when using the desktop.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        systemmenu - The process used to access the system when using the desktop.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        terminal - A GUI terminal.\n\r"
            FOREGROUND_COLOR(255, 255, 255)"        tty - A terminal like process that starts at boot.\n\r"
            ),           
            Manual("suicide", "Send a request to kill the process that called the command.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    suicide - Send a request to kill the process that called the command.\n\r"
            ),           
            Manual("restart", "Restarts the pc.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    restart - Restarts the pc.\n\r"
            ),           
            Manual("shutdown", "Shuts down the pc.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    shutdown - Shuts down the pc.\n\r"
            ),    
            Manual("heapvis", "Shows a visualization of all the segments in the heap.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    heapvis - Shows a visualization of all the segments in the heap.\n\r"
            ),           
            Manual("sysfetch", "A neofetch lookalike to give system information.",
            FOREGROUND_COLOR(086, 182, 194)"\nNAME:\n\r"
            FOREGROUND_COLOR(255, 255, 255)"    sysfetch - A neofetch lookalike to give system information.\n\r"
            ), 
        };  

        uint64_t Hash = STL::HashWord(STL::NextWord(Command));
        if (Hash == 0)
        {
            char* CurrentLocation = CommandOutput;
            CurrentLocation = STL::CopyString(CurrentLocation, "\nType help [COMMAND] for information about the specified COMMAND.\n\n\r") + 1;
            CurrentLocation = STL::CopyString(CurrentLocation, FOREGROUND_COLOR(224, 108, 117)) + 1;
            CurrentLocation = STL::CopyString(CurrentLocation, "COMMAND: \n\r" FOREGROUND_COLOR(255, 255, 255)) + 1;

            for (uint32_t i = 0; i < sizeof(Manuals)/sizeof(Manuals[0]); i++)
            {                
                CurrentLocation = STL::CopyString(CurrentLocation, "    ") + 1;
                CurrentLocation = STL::CopyString(CurrentLocation, Manuals[i].Name) + 1;
                CurrentLocation = STL::CopyString(CurrentLocation, " - ") + 1;
                CurrentLocation = STL::CopyString(CurrentLocation, Manuals[i].Description) + 1;
                CurrentLocation = STL::CopyString(CurrentLocation, "\r\n") + 1;
            }
            *CurrentLocation = 0;
            return CommandOutput;
        }

        for (uint32_t i = 0; i < sizeof(Manuals)/sizeof(Manuals[0]); i++)
        {   
            if (Manuals[i].Hash == Hash)
            {
                return Manuals[i].String;
            }
        }   

        return "ERROR: Invalid value of COMMAND ";
    }

    const char* CommandTime(const char* Command)
    {    
        uint64_t Hour = RTC::GetHour();
        uint64_t Minute = RTC::GetMinute();
        uint64_t Second = RTC::GetSecond();

        CommandOutput[0] = '0' + (Hour / 10); 
        CommandOutput[1] = '0' + (Hour % 10); 
        CommandOutput[2] = ':'; 
        CommandOutput[3] = '0' + (Minute / 10); 
        CommandOutput[4] = '0' + (Minute % 10); 
        CommandOutput[5] = ':'; 
        CommandOutput[6] = '0' + (Second / 10); 
        CommandOutput[7] = '0' + (Second % 10); 
        CommandOutput[8] = 0; 

        return CommandOutput;
    }

    const char* CommandDate(const char* Command)
    {            
        uint64_t Day = RTC::GetDay();
        uint64_t Month = RTC::GetMonth();
        uint64_t Year = RTC::GetYear();

        CommandOutput[0] = '0' + (Day / 10); 
        CommandOutput[1] = '0' + (Day % 10); 
        CommandOutput[2] = '/'; 
        CommandOutput[3] = '0' + (Month / 10); 
        CommandOutput[4] = '0' + (Month % 10); 
        CommandOutput[5] = '/'; 
        CommandOutput[6] = '2'; 
        CommandOutput[7] = '0'; 
        CommandOutput[8] = '0' + (Year / 10); 
        CommandOutput[9] = '0' + (Year % 10); 
        CommandOutput[10] = 0; 

        return CommandOutput;
    }

    const char* CommandKill(const char* Command)
    {
        Process* FoundProcess = ProcessHandler::GrandFatherProcess->FindFamilyMember(STL::NextWord(Command));

        if (FoundProcess != nullptr)
        {
            FoundProcess->PushRequest(STL::PROR::KILL);
            return "Process Killed";
        }
        else
        {
            return "ERROR: Unable to find process";
        }
    }

    const char* CommandSuicide(const char* Command)
    {
        if (ProcessHandler::ActiveProcess != nullptr)
        {
            ProcessHandler::ActiveProcess->PushRequest(STL::PROR::KILL);
        }
        return "";
    }

    const char* CommandSetFont(const char* Command)
    {
        if (STL::SetFont(STL::NextWord(Command)))
        {
            return "Font set";
        }
        else
        {
            return "ERROR: Font not found";
        }
        return "";
    }    

    const char* CommandClear(const char* Command)
    {
        if (ProcessHandler::ActiveProcess != nullptr)
        {
            ProcessHandler::ActiveProcess->PushRequest(STL::PROR::CLEAR);
        }
        return "";
    }    

    const char* CommandShutdown(const char* Command)
    {
        STL::Point ScreenSize = Renderer::GetScreenSize();

        Renderer::Clear();

        Renderer::Background = STL::ARGB(0);
        Renderer::Foreground = STL::ARGB(255, 255, 0, 0);

        Renderer::CursorPos = STL::Point(ScreenSize.X / 2 - (14 * 8 * 3) / 2, ScreenSize.Y / 3);
        Renderer::Print("Please wait...", 3);

        asm("CLI");

        UEFI::GetRT()->ResetSystem(EfiResetType::Shutdown, 0, 0, nullptr);

        Renderer::Clear();

        Renderer::Background = STL::ARGB(0);
        Renderer::Foreground = STL::ARGB(255);

        Renderer::CursorPos = STL::Point(ScreenSize.X / 2 - (40 * 8 * 3) / 2, ScreenSize.Y / 3);
        Renderer::Print("It is now safe to turn off your computer", 3);

        while (true)
        {
            asm("HLT");
        }

        return "";
    }  

    const char* CommandRestart(const char* Command)
    {       
        STL::Point ScreenSize = Renderer::GetScreenSize();

        Renderer::Clear();

        Renderer::Background = STL::ARGB(0);
        Renderer::Foreground = STL::ARGB(255, 255, 0, 0);

        Renderer::CursorPos = STL::Point(ScreenSize.X / 2 - (14 * 8 * 3) / 2, ScreenSize.Y / 3);
        Renderer::Print("Please wait...", 3);
        
        asm("CLI");

        UEFI::GetRT()->ResetSystem(EfiResetType::Cold, 0, 0, nullptr);

        Renderer::Clear();

        Renderer::Background = STL::ARGB(0);
        Renderer::Foreground = STL::ARGB(255);

        Renderer::CursorPos = STL::Point(ScreenSize.X / 2 - (40 * 8 * 3) / 2, ScreenSize.Y / 3);
        Renderer::Print("It is now safe to turn off your computer", 3);

        while (true)
        {
            asm("HLT");
        }

        return "";
    }   

    const char* CommandStart(const char* Command)
    {
        struct StartableProcess
        {
            const char* Name;
            uint64_t Hash;
            STL::PROC Procedure;

            StartableProcess(const char* Name, STL::PROC Procedure)
            {
                this->Procedure = Procedure;
                this->Name = Name;
                this->Hash = STL::HashWord(Name);
            }
        };

        StartableProcess StartableProcesses[] =
        {
            StartableProcess("tty", tty::Procedure),
            StartableProcess("desktop", Desktop::Procedure),
            StartableProcess("topbar", Topbar::Procedure),
            StartableProcess("systemmenu", SystemMenu::Procedure),
            StartableProcess("startmenu", StartMenu::Procedure),
            StartableProcess("terminal", Terminal::Procedure),
            StartableProcess("calculator", Calculator::Procedure),
            StartableProcess("tetris", Tetris::Procedure)
        };

        uint64_t Hash = STL::HashWord(STL::NextWord(Command));
        for (uint32_t i = 0; i < sizeof(StartableProcesses)/sizeof(StartableProcesses[0]); i++)
        {
            if (Hash == StartableProcesses[i].Hash)
            {
                new Process(StartableProcesses[i].Procedure, nullptr);
                return "Process Started";
            }
        }

        return "ERROR: Process not found";
    }

    const char* CommandHeapvis(const char* Command)
    {    
        char* Index = CommandOutput;
        auto Write = [&](const char* String)
        {
            Index = STL::CopyString(Index, String) + 1;
            Index[0] = 0;
        };

        Write(FOREGROUND_COLOR(000, 000, 000));

        Heap::Segment* CurrentSegment = Heap::GetFirstSegment();
        while (true)
        {
            if (CurrentSegment->Free)
            {
                Write(BACKGROUND_COLOR(152, 195, 121));
            }
            else
            {
                Write(BACKGROUND_COLOR(224, 108, 117));
            }

            Write(" ");

            if (CurrentSegment->Size >= 1000)
            {
                Write(STL::ToString(CurrentSegment->Size / 1000));
                Write(" KB");  
            }
            else
            {
                Write(STL::ToString(CurrentSegment->Size));
                Write(" B");
            }

            Write(" ");
            Write(BACKGROUND_COLOR(000, 000, 000));
            Write(" ");

            if (CurrentSegment->Next == nullptr)
            {
                break;
            }
            CurrentSegment = CurrentSegment->Next;
        }

        Write(FOREGROUND_COLOR(255, 255, 255));
        Write(BACKGROUND_COLOR(000, 000, 000));

        return CommandOutput;
    }

    const char* CommandSysfetch(const char* Command)
    {                
        const char* Sysfetch =                                                   
        FOREGROUND_COLOR(086, 182, 194)"         /ooooooooooo/                                                                                                      \n\r" 
        FOREGROUND_COLOR(086, 182, 194)"        /ooooooooooo/                                                                                                       \n\r"
        FOREGROUND_COLOR(086, 182, 194)"       /ooooooooooo/                                                                                                        \n\r"
        FOREGROUND_COLOR(086, 182, 194)"      /oooooooooo/                                                                                                          \n\r"
        FOREGROUND_COLOR(086, 182, 194)"     /oooooooooo/                                                                                                           \n\r"
        FOREGROUND_COLOR(086, 182, 194)"    /oooooooooo/                                                                                                            \n\r"
        FOREGROUND_COLOR(086, 182, 194)"   /oooooooooo/                                                                                                             \n\r"
        FOREGROUND_COLOR(086, 182, 194)"  /ooooooooooooooooooooo/                                                                                                   \n\r"
        FOREGROUND_COLOR(086, 182, 194)" /ooooooooooooooooooooo/                                                                                                    \n\r"
        FOREGROUND_COLOR(086, 182, 194)" /oooooooooooooooooooo/                                                                                                     \n\r"
        FOREGROUND_COLOR(086, 182, 194)"/ooooooooooooooooooo/                                                                                                       \n\r"
        FOREGROUND_COLOR(086, 182, 194)"           /ooooooo/                                                                                                        \n\r"
        FOREGROUND_COLOR(086, 182, 194)"          /oooooo/                                                                                                          \n\r"
        FOREGROUND_COLOR(086, 182, 194)"         /oooooo/                                                                                                           \n\r"
        FOREGROUND_COLOR(086, 182, 194)"         /oooo/                                                                                                             \n\r"
        FOREGROUND_COLOR(086, 182, 194)"        /oooo/                                                                                                              \n\r"
        FOREGROUND_COLOR(086, 182, 194)"       /oooo/                                                                                                               \n\r"
        FOREGROUND_COLOR(086, 182, 194)"      /ooo/                                                                                                                 \n\r"
        FOREGROUND_COLOR(255, 255, 255);

        auto Write = [&](uint64_t Line, const char* Part1, const char* Part2 = nullptr, const char* Part3 = nullptr) 
        { 
            char* Temp = (char*)((uint64_t)Sysfetch + Line * (11 + 126) + 11 + 27);
            Temp = STL::CopyString(Temp, Part1) + 1;
            if (Part2 != nullptr)
            {            
                Temp = STL::CopyString(Temp, FOREGROUND_COLOR(255, 255, 255)) + 1;
                Temp = STL::CopyString(Temp, Part2) + 1;
            }
            if (Part3 != nullptr)
            {            
                Temp = STL::CopyString(Temp, Part3) + 1;
            }
        }; 

        Write(1, "OS: ", OS_VERSION);
        Write(2, "Time: ", CommandTime(nullptr));
        Write(3, "Date: ", CommandDate(nullptr));
        Write(4, "Ticks: ", STL::ToString(PIT::Ticks));
        Write(5, "Uptime: ", STL::ToString(PIT::Ticks / PIT::GetFrequency()), " s   ");
        Write(6, "Free Heap: ", STL::ToString(Heap::GetFreeSize() / 1000), " KB   ");
        Write(7, "Used Heap: ", STL::ToString(Heap::GetUsedSize() / 1000), " KB   ");
        Write(8, "Total Heap: ", STL::ToString((Heap::GetUsedSize() + Heap::GetFreeSize()) / 1000), " KB   ");
        Write(9, "Heap Segments: ", STL::ToString(Heap::GetSegmentAmount()));
        //Write(10, "Process Amount: ", STL::ToString(ProcessHandler::Processes.Length()));

        Write(14, "\033B040044052   \033B224108117   \033B229192123   \033B152195121   \033B097175239   \033B198120221   \033B000000000");
        Write(15, "\033B040044052   \033B224108117   \033B229192123   \033B152195121   \033B097175239   \033B198120221   \033B000000000");
         
        return Sysfetch;
    }   

    const char* System(const char* Input)
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
                this->Hash = STL::HashWord(Name);
            }
        };

        Command Commands[] =
        {
            Command("set", CommandSet),
            Command("list", CommandList),
            Command("help", CommandHelp),
            Command("time", CommandTime),
            Command("date", CommandDate),
            Command("kill", CommandKill),
            Command("clear", CommandClear),
            Command("setfont", CommandSetFont),
            Command("start", CommandStart),
            Command("restart", CommandRestart),
            Command("shutdown", CommandShutdown),
            Command("suicide", CommandSuicide),
            Command("heapvis", CommandHeapvis),
            Command("sysfetch", CommandSysfetch)
        };

        uint64_t Hash = STL::HashWord(Input);
        for (uint32_t i = 0; i < sizeof(Commands)/sizeof(Commands[0]); i++)
        {
            if (Hash == Commands[i].Hash)
            {
                return Commands[i].Function(Input);
            }
        }

        return "ERROR: Command not found";
    }

    STL::SYSRV Call(uint64_t Selector...)
    {
        va_list Args;
        va_start(Args, Selector);

        STL::SYSRV ReturnVal = 0;

        switch (Selector)
        {
        case 0:
        {
            ReturnVal = (STL::SYSRV)System::System(va_arg(Args, const char*));
        }
        break;
        case 1:
        {
            ReturnVal = (STL::SYSRV)Heap::Allocate(va_arg(Args, uint64_t));
        }
        break;
        case 2:
        {
            Heap::Free(va_arg(Args, void*));
        }
        break;
        }

        va_end(Args);

        return ReturnVal;
    }
}