#pragma once

#include <stdint.h>

struct PageIndexer 
{
    uint64_t PDP;

    uint64_t PD;

    uint64_t PT;

    uint64_t P;

    PageIndexer(uint64_t VirtualAddress);
};