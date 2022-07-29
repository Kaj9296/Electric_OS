#include "DeviceHeader.h"

#include "STL/String/cstr.h"

const char* DeviceHeader::GetVendorString()
{
    switch (this->VendorID)
    {
    case 0x8086:
    {
        return "Intel Corp";
    }
    break;
    case 0x1022:
    {
        return "AMD";
    }
    break;
    case 0x10DE:
    {
        return "NVIDIA";
    }
    break;
    case 0x1C5C:
    {
        return "SK Hynix";
    }
    break;
    case 0x10EC:
    {
        return "REALTEK";
    }
    break;
    default:
    {
        return STL::ToString(this->VendorID);
    }
    break;
    }
}

const char* DeviceHeader::GetTypeString()
{
    switch (this->Class)
    {
    case 0x01:
    {
        switch (this->Subclass)
        {  
        case 0x00:
        {
            return "SCSI Bus Controller";
        }
        break;
        case 0x01:
        {
            return "IDE Controller";
        }
        break;
        case 0x02:
        {
            return "Floppy Disk Controller";
        }
        break;
        case 0x03:
        {
            return "IPI Bus Controller";
        }
        break;
        case 0x04:
        {
            return "RAID Controller";
        }
        break;
        case 0x05:
        {
            return "ATA Controller";
        }
        break;
        case 0x06:
        {
            return "Serial ATA";
        }
        break;
        case 0x07:
        {
            return "Serial Attached SCSI";
        }
        break;
        case 0x08:
        {
            return "Non-Volatile Memory Controller";
        }
        break;
        default:
        {
            return STL::ToString(this->Subclass);
        }
        break;
        }
    }
    break;
    case 0x03:
    {
        switch (this->Subclass)
        {
        case 0x00:
        {
            return "VGA Compatible Controller";
        }
        break;
        default:
        {
            return STL::ToString(this->Subclass);
        }
        break;
        }
    }
    break;
    case 0x06:
    {
        switch (this->Subclass)
        {
        case 0x00:
        {
            return "Host Bridge";
        }
        break;
        case 0x01:
        {
            return "ISA Bridge";
        }
        break;
        case 0x02:
        {
            return "EISA Bridge";
        }
        break;
        case 0x03:
        {
            return "MCA Bridge";
        }
        break;
        case 0x04:
        {
            return "PCI-to-PCI Bridge";
        }
        break;
        case 0x05:
        {
            return "PCMCIA Bridge";
        }
        break;
        case 0x06:
        {
            return "NuBus Bridge";
        }
        break;
        case 0x07:
        {
            return "CardBus Bridge";
        }
        break;
        case 0x08:
        {
            return "RACEway Bridge";
        }
        break;
        case 0x09:
        {
            return "PCI-to-PCI Bridge";
        }
        break;
        case 0x0a:
        {
            return "InfiniBand-to-PCI Host Bridge";
        }
        break;
        case 0x80:
        {
            return "Other";
        }
        break;
        default:
        {
            return STL::ToString(this->Subclass);
        }
        break;
        }
    }
    break;
    case 0x0C: 
    {
        switch (this->Subclass)
        {
        case 0x00:
        {
            return "FireWire (IEEE 1394) Controller";
        }
        break;
        case 0x01:
        {
            return "ACCESS Bus";
        }
        break;
        case 0x02:
        {
            return "SSA";
        }
        break;
        case 0x03:
        {
            return "USB Controller";
        }
        break;
        case 0x04:
        {
            return "Fibre Channel";
        }
        break;
        case 0x05:
        {
            return "SMBus";
        }
        break;
        case 0x06:
        {
            return "Infiniband";
        }
        break;
        case 0x07:
        {
            return "IPMI Interface";
        }
        break;
        case 0x08:
        {
            return "SERCOS Interface (IEC 61491)";
        }
        break;
        case 0x09:
        {
            return "CANbus";
        }
        break;
        case 0x80:
        {
            return "SerialBusController - Other";
        }
        break;
        default:
        {
            return STL::ToString(this->Subclass);
        }
        break;
        }
    }
    break;
    default:
    {
        return STL::ToString(this->Subclass);
    }
    break;
    }

    return STL::ToString(this->Subclass);
}