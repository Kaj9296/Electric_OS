#pragma once

#include <stdint.h>

namespace RTC
{
    uint8_t GetSecond();

    uint8_t GetMinute();

    uint8_t GetHour();

    uint8_t GetDay();

    uint8_t GetMonth();

    uint8_t GetYear();

    void Wait();

    uint8_t GetRegister(uint32_t Register);

    void Update();
}