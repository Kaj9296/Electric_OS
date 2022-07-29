#include "IDT.h"
#include "Handlers.h"
#include "Kernel/IO/IO.h"
#include "Kernel/Input/Mouse.h"
#include "Kernel/Memory/Paging/PageAllocator.h"

namespace IDT
{
    void IDTEntry::SetOffset(uint64_t Offset)
    {
        this->Offset0 = (uint16_t)(Offset & 0x000000000000ffff);
        this->Offset1 = (uint16_t)((Offset & 0x00000000ffff0000) >> 16);
        this->Offset2 = (uint32_t)((Offset & 0xffffffff00000000) >> 32);
    }

    uint64_t IDTEntry::GetOffset()
    {
        uint64_t Temp = 0;
        Temp |= (uint64_t)this->Offset0;
        Temp |= (uint64_t)this->Offset1 << 16;
        Temp |= (uint64_t)this->Offset2 << 32;
        return Temp;
    }

    void IDTR::SetHandler(uint64_t VectorNR, uint64_t HandlerOffset)
    {
        IDTEntry* PageFault_Int = (IDTEntry*)(this->Offset + VectorNR * sizeof(IDTEntry));
        PageFault_Int->SetOffset(HandlerOffset);
        PageFault_Int->TypeAttributes = IDT_TA_InterruptGate;
        PageFault_Int->Selector = 0x08;
    }

    void EnableInterrupts()
    {
        IO::OutByte(PIC1_DATA, 0b11111000);
        IO::OutByte(PIC2_DATA, 0b11101111);
    }

    void DisableInterrupts()
    {
        IO::OutByte(PIC1_DATA, 0b11111111);
        IO::OutByte(PIC2_DATA, 0b11111111);
    }

    void SetupInterrupts()
    {
        static IDTR idtr;
        static IDTEntry _IDT[256];
        
        idtr.Limit = 0x0FFF;
        idtr.Offset = (uint64_t)_IDT;

        idtr.SetHandler(0x0, (uint64_t)InteruptHandlers::DivideByZero);
        idtr.SetHandler(0x2, (uint64_t)InteruptHandlers::NoneMaskableInterrupt);
        idtr.SetHandler(0x3, (uint64_t)InteruptHandlers::Breakpoint);
        idtr.SetHandler(0x4, (uint64_t)InteruptHandlers::Overflow);
        idtr.SetHandler(0x5, (uint64_t)InteruptHandlers::BoundRange);
        idtr.SetHandler(0x6, (uint64_t)InteruptHandlers::InvalidOP);
        idtr.SetHandler(0x7, (uint64_t)InteruptHandlers::DeviceNotDetected);
        idtr.SetHandler(0x8, (uint64_t)InteruptHandlers::DoubleFault);
        idtr.SetHandler(0xA, (uint64_t)InteruptHandlers::InvalidTSS);
        idtr.SetHandler(0xB, (uint64_t)InteruptHandlers::SegmentNotPresent);
        idtr.SetHandler(0xC, (uint64_t)InteruptHandlers::StackSegmentFault);
        idtr.SetHandler(0xD, (uint64_t)InteruptHandlers::GeneralProtectionFault);
        idtr.SetHandler(0xE, (uint64_t)InteruptHandlers::PageFault);
        idtr.SetHandler(0x10, (uint64_t)InteruptHandlers::FloatingPoint);

        idtr.SetHandler(0x20, (uint64_t)InteruptHandlers::PIT);
        idtr.SetHandler(0x21, (uint64_t)InteruptHandlers::Keyboard);
        idtr.SetHandler(0x2C, (uint64_t)InteruptHandlers::Mouse);

        asm("LIDT %0" : : "m" (idtr));

        uint8_t a1 = IO::InByte(PIC1_DATA);
        IO::Wait();
        uint8_t a2 = IO::InByte(PIC2_DATA);
        IO::Wait();

        IO::OutByte(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
        IO::Wait();
        IO::OutByte(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
        IO::Wait();

        IO::OutByte(PIC1_DATA, 0x20);
        IO::Wait();
        IO::OutByte(PIC2_DATA, 0x28);
        IO::Wait();

        IO::OutByte(PIC1_DATA, 4);
        IO::Wait();
        IO::OutByte(PIC2_DATA, 2);
        IO::Wait();

        IO::OutByte(PIC1_DATA, ICW4_8086);
        IO::Wait();
        IO::OutByte(PIC2_DATA, ICW4_8086);
        IO::Wait();

        IO::OutByte(PIC1_DATA, a1);
        IO::Wait();
        IO::OutByte(PIC2_DATA, a2);
        IO::Wait();

        Mouse::InitPS2();

        EnableInterrupts();

        asm ("sti");
    }
}
