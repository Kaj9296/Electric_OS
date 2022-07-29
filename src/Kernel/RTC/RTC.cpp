#include "RTC.h"

#include "Kernel/IO/IO.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define TIME_ZONE

namespace RTC
{
    uint8_t Second;

    uint8_t Minute;

    uint8_t Hour;

    uint8_t Day;

    uint8_t Month;

    uint8_t Year;

    uint8_t GetSecond()
    {
        return Second;
    }

    uint8_t GetMinute()
    {
        return Minute;
    }

    uint8_t GetHour()
    {
        return Hour;
    }

    uint8_t GetDay()
    {
        return Day;
    }

    uint8_t GetMonth()
    {
        return Month;
    }

    uint8_t GetYear()
    {
        return Year;
    }

    void Wait()
    {
        while (GetRegister(0x80) & 0x80);
    }

    uint8_t GetRegister(uint32_t Register)
    {
        IO::OutByte(CMOS_ADDRESS, Register);
        return IO::InByte(CMOS_DATA);
    }

    void Update()
    {
        Wait();

        Second = GetRegister(0x00);
        Minute = GetRegister(0x02);
        Hour = GetRegister(0x04);
        Day = GetRegister(0x07);
        Month = GetRegister(0x08);
        Year = GetRegister(0x09);

        uint8_t RegisterB = GetRegister(0x0B);
  
        if (!(RegisterB & 0x04)) 
        {
            Second = (Second & 0x0F) + ((Second / 16) * 10);
            Minute = (Minute & 0x0F) + ((Minute / 16) * 10);
            Hour = ((Hour & 0x0F) + (((Hour & 0x70) / 16) * 10)) | (Hour & 0x80);
            Day = (Day & 0x0F) + ((Day / 16) * 10);
            Month = (Month & 0x0F) + ((Month / 16) * 10);
            Year = (Year & 0x0F) + ((Year / 16) * 10);
        }
    }
}