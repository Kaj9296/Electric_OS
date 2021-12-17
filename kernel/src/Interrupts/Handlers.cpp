#include "Handlers.h"
#include "IDT.h"
#include "../Rendering/Renderer.h"
#include "../Core/Panic.h"
#include "../IO/IO.h"
#include "../UserInput/KeyBoard.h"
#include "../UserInput/Mouse.h"

namespace InteruptHandlers
{
    __attribute__((interrupt)) void InvalidOP(InterruptFrame* frame)
    {
        Panic("Invalid OP Code Detected");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void DeviceNotDetected(InterruptFrame* frame)
    {
        Panic("Device Not Detected");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void DoubleFault(InterruptFrame* frame)
    {
        Panic("Double Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void SegmentNotPresent(InterruptFrame* frame)
    {
        Panic("Segment Not Present");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void StackSegmentFault(InterruptFrame* frame)
    {
        Panic("Stack Segment Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void GeneralProtectionFault(InterruptFrame* frame)
    {
        Panic("General Protection Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void PageFault(InterruptFrame* frame)
    {
        Panic("Page Fault");
        while(true)
        {
            asm("HLT");
        }
    }

    __attribute__((interrupt)) void Keyboard(InterruptFrame* frame)
    {
        uint8_t ScanCode = IO::InByte(0x60);

        KeyBoard::HandleScanCode(ScanCode);

        uint8_t Key = KeyBoard::GetKeyPress();
        if (Key != 0)
        {
            Renderer::Print(Key);
            Renderer::SwapBuffers();
        }

        IO::OutByte(PIC1_COMMAND, PIC_EOI);
    }

    __attribute__((interrupt)) void Mouse(InterruptFrame* frame)
    {
        uint8_t MouseData = IO::InByte(0x60);

        Mouse::HandleMouseData(MouseData);

        IO::OutByte(PIC2_COMMAND, PIC_EOI);
        IO::OutByte(PIC1_COMMAND, PIC_EOI);
    }
}