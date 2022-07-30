#include "PCI.h"

#include "Kernel/Memory/Paging/PageTable.h"

namespace PCI
{       
    uint8_t Entry = 0;
    uint8_t Bus = 0;
    uint8_t Device = 0;
    uint8_t Func = 0;

    SDTHeader* MCFG;

    void Init()
    {
        ResetEnumeration();
        MCFG = ACPI::FindTable("MCFG");
    }

    void ResetEnumeration()
    {       
        Entry = 0;
        Bus = 0;
        Device = 0;
        Func = 0;
    }

    bool Enumerate(DeviceHeader*& Out)
    {        
        if (Out == nullptr)
        {
            ResetEnumeration();
        }

        uint64_t Entries = (MCFG->Length - (sizeof(SDTHeader) + 8)) / sizeof(DeviceConfig);

        for (; Entry < Entries; Entry++)
        {
            DeviceConfig* NewDeviceConfig = (DeviceConfig*)((uint64_t)MCFG + (sizeof(SDTHeader) + 8) + Entry * sizeof(DeviceConfig));

            if (Bus == 0)
            {
                Bus = NewDeviceConfig->StartBus;
            }

            for (; Bus < NewDeviceConfig->EndBus; Bus++)
            {
                uint64_t BusAddress = NewDeviceConfig->Base + (Bus << 20);

                PageTableManager::MapAddress((void*)BusAddress, (void*)BusAddress);

                DeviceHeader* BusHeader = (DeviceHeader*)BusAddress;

                if (BusHeader->DeviceID == 0 || BusHeader->DeviceID == 0xFFFF)
                {
                    continue;
                }    

                for (; Device < 32; Device++)
                {
                    uint64_t DeviceAddress = BusAddress + (Device << 15);

                    PageTableManager::MapAddress((void*)DeviceAddress, (void*)DeviceAddress);

                    DeviceHeader* DevHeader = (DeviceHeader*)DeviceAddress;

                    if (DevHeader->DeviceID == 0 || DevHeader->DeviceID == 0xFFFF)
                    {
                        continue;
                    }

                    for (; Func < 8; Func++)
                    {
                        uint64_t FunctionAddress = DeviceAddress + (Func << 12);

                        PageTableManager::MapAddress((void*)FunctionAddress, (void*)FunctionAddress);

                        DeviceHeader* FuncHeader = (DeviceHeader*)FunctionAddress;

                        if (FuncHeader->DeviceID == 0 || FuncHeader->DeviceID == 0xFFFF)
                        {
                            continue;
                        }       
                        
                        Func++;
                        Out = FuncHeader;
                        return true;
                    }

                    Func = 0;
                }

                Device = 0;
            }

            Bus = 0;
        }

        Entry = 0;

        return false;
    }
}