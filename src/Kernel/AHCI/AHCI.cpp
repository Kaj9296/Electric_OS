#include "AHCI.h"
#include "Kernel/PCI/PCI.h"
#include "Kernel/Memory/Paging/PageTable.h"
#include "Kernel/Renderer/Renderer.h"

namespace AHCI
{
    PCIHeader* ACHIDevice;
    HBAMemory* ABAR;

    void Init()
    {
        DeviceHeader* Device;
        PCI::ResetEnumeration();
        while (PCI::Enumerate(Device))
        { 
            if (Device->Class == 0x01 && Device->Subclass == 0x06 && Device->ProgIF == 0x01)
            {
                ACHIDevice = (PCIHeader*)Device;
                break;
            }
        }

        ABAR = (HBAMemory*)ACHIDevice->BAR5;
        PageTableManager::MapAddress(ABAR, ABAR);   
    }    

    HBAMemory* GetABAR()
    {
        return ABAR;
    }
}