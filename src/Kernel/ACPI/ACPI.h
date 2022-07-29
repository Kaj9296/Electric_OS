#pragma once

#include <stdint.h>

/// <summary>
/// (Root System Description Pointer Version 2)
/// </summary>
struct RSDP2
{
    uint8_t Signature[8];
    uint8_t CheckSum;
    uint8_t OEMID[6];
    uint8_t Revision;
    uint32_t RSDTAddress;
    uint32_t Length;
    uint64_t XSDTAddress;
    uint8_t ExtendedCheckSum;
    uint8_t Reserved[3];
} __attribute__((packed));

/// <summary>
/// (System Descriptor Table Header) 
/// </summary>
struct SDTHeader
{
    uint8_t Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t CheckSum;
    uint8_t OEMID[6];
    uint8_t OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision; 
} __attribute__((packed));

struct DeviceConfig
{
    uint64_t Base;
    uint16_t PCISegGroup;
    uint8_t StartBus;
    uint8_t EndBus;
    uint32_t Reserved;
} __attribute__((packed));

namespace ACPI
{
    void Init(RSDP2* In_RSDP);

    SDTHeader* FindTable(const char* Signature);
}