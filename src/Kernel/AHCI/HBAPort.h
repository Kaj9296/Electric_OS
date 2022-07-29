#pragma once

#include <stdint.h>

enum class HBAPortType 
{
    NONE = 0,
    SATA = 1,
    SEMB = 2,
    PM = 3,
    SATAPI = 4,
};

struct HBAPort
{
    uint32_t CommandListBase;
    uint32_t CommandListBaseUpper;
    uint32_t FisBaseAddress;
    uint32_t FisBaseAddressUpper;
    uint32_t InterruptStatus;
    uint32_t InterruptEnable;
    uint32_t CMDSts;
    uint32_t Reserve0;
    uint32_t TaskFileData;
    uint32_t Signature;
    uint32_t SATAStatus;
    uint32_t SATAControl;
    uint32_t SATAError;
    uint32_t SATAActive;
    uint32_t CommandIssue;
    uint32_t SataNotification;
    uint32_t FisSwitchControl;
    uint32_t Reserve1[11];
    uint32_t Vendor[4];

    HBAPortType GetPortType();

} __attribute__((packed));