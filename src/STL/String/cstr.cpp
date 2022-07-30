#include "cstr.h"
#include "STL/Math/Math.h"

namespace STL
{
    char IntToStringOutput[128];
    char* ToString(uint64_t Number)
    {
        uint32_t DigitAmount = STL::GetDigitAmount(Number);

        if (DigitAmount == 0)
        {
            IntToStringOutput[0] = '0';
            IntToStringOutput[1] = 0;           
        }
        else
        {
           for (uint32_t i = DigitAmount; i-- > 0;)
            {
                IntToStringOutput[DigitAmount - i - 1] = '0' + STL::GetDigit(Number, i);
            }
            IntToStringOutput[DigitAmount] = '\0';         
        }

        return IntToStringOutput;
    }

    uint64_t ToInt(const char* String)
    {
        uint64_t Result = 0;
        uint64_t Multiplier = 1;
        for (int i = Length(String) - 1; i >= 0; i--)
        {
            Result += (String[i] - '0') * Multiplier;
            Multiplier *= 10;
        }

        return Result;
    }

    char* NextWord(const char* String)
    {
        uint64_t i = 0;
        while (true)
        {
            i++;
            if (String[i] == 0)
            {
                i--;
                break;
            }
            else if (String[i] == ' ')
            {
                i++;
                break;
            }
        }
        return (char*)(String + i);
    }

    uint64_t LineAmount(const char* String)
    {
        uint64_t Amount = 1;
        char* S = (char*)String;
        while (*S != 0)
        {
            if (*S == '\n')
            {
                Amount++;
            }
            S++;
        }
        return Amount;
    }

    uint64_t HashWord(const char* String)
    {
        uint64_t Result = 0;
        uint64_t i = 0;
        while (true)
        {
            Result += ((uint64_t)(String[i] & 0x0F)) << (4 * i);

            i++;
            if (String[i] == 0 || String[i] == ' ')
            {
                break;
            }
        }

        return Result;
    }
        
    uint64_t Length(const char* String)
    {
        uint32_t i = 0;
        while (true)
        {
            if (String[i] == 0)
            {
                return i;
            }
            i++;
        }

        return 0;
    }

    bool StringCompare(const char* String1, const char* String2)
    {
        uint64_t String1Length = Length(String1);
        uint64_t String2Length = Length(String2);

        if (String1Length != String2Length)
        {
            return false;
        }

        for (int i = 0; i < String1Length; i++)
        {
            if (String1[i] != String2[i])
            {
                return false;
            }
        }

        return true;
    }

    char* CopyString(char* Dest, const char* Source)
    {
        uint64_t SourceLength = Length(Source);

        for (uint32_t i = 0; i < SourceLength; i++)
        {
            Dest[i] = Source[i];
        }

        return Dest + (SourceLength - 1);
    }
}
