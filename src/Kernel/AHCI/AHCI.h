#pragma once
#include <stdint.h>
#include "HBAPort.h"

struct HBAMemory
{
    uint32_t HostCapability;
    uint32_t GlobalHostControl; 
    uint32_t InterruptStatus;
    uint32_t PortsImplemented;
    uint32_t Version;
    uint32_t CCCControl;
    uint32_t CCCPorts;
    uint32_t EnclosureManagementLocation;
    uint32_t EnclosureManagementControl;
    uint32_t HostCapabilitiesExtended;
    uint32_t BiosHandoffCtrlSts;
    uint8_t ReserveV0[0x74];
    uint8_t Vendor[0x60];
    HBAPort Ports[1];
} __attribute__((packed));

namespace AHCI
{    
    void Init();

    HBAMemory* GetABAR();
}