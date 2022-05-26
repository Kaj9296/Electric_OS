#include "Handlers.h"
#include "IDT.h"
#include "Renderer/Renderer.h"
#include "Debug/Debug.h"
#include "IO/IO.h"
#include "RTC/RTC.h"
#include "Input/KeyBoard.h"
#include "Input/Mouse.h"
#include "PIT/PIT.h"
#include "ProcessHandler/ProcessHandler.h"

namespace InteruptHandlers
{        
    __attribute__((interrupt)) void DivideByZero(InterruptFrame* frame)
    {
        Debug::Error("Division By Zero Detected");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void NoneMaskableInterrupt(InterruptFrame* frame)
    {
        Debug::Error("None Maskable Interrupt");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void Breakpoint(InterruptFrame* frame)
    {
        Debug::Error("Breakpoint reached");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void Overflow(InterruptFrame* frame)
    {
        Debug::Error("Overflow detected");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void BoundRange(InterruptFrame* frame)
    {
        Debug::Error("Bound Range Exceeded");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void InvalidOP(InterruptFrame* frame)
    {
        Debug::Error("Invalid OP Code Detected");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void DeviceNotDetected(InterruptFrame* frame)
    {
        Debug::Error("Device Not Detected");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void DoubleFault(InterruptFrame* frame)
    {
        Debug::Error("Double Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void InvalidTSS(InterruptFrame* frame)
    {
        Debug::Error("Invalid TSS");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void SegmentNotPresent(InterruptFrame* frame)
    {
        Debug::Error("Segment Not Present");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void StackSegmentFault(InterruptFrame* frame)
    {
        Debug::Error("Stack Segment Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void GeneralProtectionFault(InterruptFrame* frame)
    {
        Debug::Error("General Protection Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void PageFault(InterruptFrame* frame)
    {
        Debug::Error("Page Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void FloatingPoint(InterruptFrame* frame)
    {
        Debug::Error("Floating Point Exception");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void PIT(InterruptFrame* frame)
    {
        static uint64_t OldRTCTick = 0;

        PIT::Tick();

        /// If multiple of one second update time and date.
        if (PIT::Ticks >= OldRTCTick + 100)
        {
            RTC::Update();
            OldRTCTick = PIT::Ticks;
        }

        /// Notify processes of interupt.
        ProcessHandler::PITInterupt(PIT::Ticks);

        IO::OutByte(PIC1_COMMAND, PIC_EOI);
    }

    __attribute__((interrupt)) void Keyboard(InterruptFrame* frame)
    {        
        uint8_t ScanCode = IO::InByte(0x60);

        KeyBoard::HandleScanCode(ScanCode);

        if (!(ScanCode & (0b10000000))) //If key was pressed down
        {
            ProcessHandler::KeyBoardInterupt();
        }

        /// Notify processes of interupt.
        IO::OutByte(PIC1_COMMAND, PIC_EOI);
    }

    __attribute__((interrupt)) void Mouse(InterruptFrame* frame)
    {                
        uint8_t MouseByte = IO::InByte(0x60);

        Mouse::HandleMouseByte(MouseByte);

        IO::OutByte(PIC2_COMMAND, PIC_EOI);
        IO::OutByte(PIC1_COMMAND, PIC_EOI);
    }    
}
