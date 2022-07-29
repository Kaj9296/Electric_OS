#pragma once

#include <stdint.h>

#include "DeviceHeader.h"

#include "Kernel/ACPI/ACPI.h"

struct PCIHeader 
{
    DeviceHeader Header;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t CardbusCISPtr;
    uint16_t SubsystemVendorID;
    uint16_t SubsystemID;
    uint32_t ExpansionROMBaseAddr;
    uint8_t CapabilitiesPtr;
    uint8_t Reserve0;
    uint16_t Reserve1;
    uint32_t Reserve2;
    uint8_t InterruptLine;
    uint8_t InterruptPin;
    uint8_t MinGrant;
    uint8_t MaxLatency;
} __attribute__((packed));

namespace PCI
{            
    void Init();

    /// <summary>
    //// Starts over enumeration to the first device.
    /// </summary>
    void ResetEnumeration();

    /// <summary>
    //// Sets out to be the next PCI device and returns true untill there are no more devices then it returns fales.
    /// </summary>
    bool Enumerate(DeviceHeader*& Out);
}