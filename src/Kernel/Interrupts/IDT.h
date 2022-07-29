#pragma once

#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define IDT_TA_InterruptGate 0b10001110
#define IDT_TA_TrapGate 0b10001111
#define IDT_TA_CallGate 0b10001100

namespace IDT
{
    struct IDTEntry
    {
        void SetOffset(uint64_t Offset);

        uint64_t GetOffset();

        uint16_t Offset0;
        uint16_t Selector;
        uint8_t Ist;
        uint8_t TypeAttributes;
        uint16_t Offset1;
        uint32_t Offset2;
        uint32_t Zero;
    };

    struct IDTR
    {
        void SetHandler(uint64_t VectorNR, uint64_t HandlerOffset);

        uint16_t Limit;
        uint64_t Offset;
    }__attribute__((packed));

    void EnableInterrupts();

    void DisableInterrupts();

    void SetupInterrupts();
}
