#include "HBAPort.h"

HBAPortType HBAPort::GetPortType()
{
    if ((uint8_t)(this->SATAStatus & 0b111) != 0x3) //Device not detected or physical communication not established.
    {
        return HBAPortType::NONE;
    }
    else if ((uint8_t)((this->SATAStatus >> 8) & 0b111) != 0x1) //Device not active.
    {
        return HBAPortType::NONE;
    }

    switch (this->Signature)
    {
    case 0xEB140101:
    {
        return HBAPortType::SATAPI;
    }
    break;
    case 0x00000101:
    {
        return HBAPortType::SATA;
    }
    break;
    case 0x96690101:
    {
        return HBAPortType::PM;
    }
    break;
    case 0xC33C0101:
    {
        return HBAPortType::SEMB;
    }
    break;
    default:
    {
        return HBAPortType::NONE;
    }
    break;
    }
}