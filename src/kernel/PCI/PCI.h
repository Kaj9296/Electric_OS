#pragma once

#include <stdint.h>

#include "kernel/ACPI/ACPI.h"

struct DeviceHeader
{
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint8_t RevisionID;
    uint8_t ProgIF;
    uint8_t Subclass;
    uint8_t Class;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
} __attribute__((packed));

namespace PCI
{        
    void ResetEnumeration();

    bool Enumerate(MCFGHeader* MCFG, DeviceHeader*& Out);
}