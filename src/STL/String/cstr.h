#pragma once
#include <stdint.h>

#define FOREGROUND_COLOR(r, g, b) "\033F"#r#g#b
#define BACKGROUND_COLOR(r, g, b) "\033B"#r#g#b
#define NEWLINE "\n\r"

namespace STL
{
    char* ToString(uint64_t Number);

    uint64_t ToInt(const char* String);

    char* NextWord(const char* String);

    uint64_t LineAmount(const char* String);

    constexpr uint64_t ConstHashWord(const char* String)
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

    uint64_t HashWord(const char* String);

    uint64_t Length(const char* String);

    bool StringCompare(const char* String1, const char* String2);
    
    char* CopyString(char* Dest, const char* Source);
}
