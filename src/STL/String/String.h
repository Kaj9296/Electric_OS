#pragma once

#include <stdint.h>

namespace STL
{
    class String
    {
    public:

        char Pop();

        char* cstr() const;

        uint32_t Length() const;

        void Erase(uint32_t Index, uint32_t Amount = 1);

        void Reserve(uint32_t MinSize);

        bool operator==(const char* Other);

        bool operator==(String& Other);

        void operator=(char const& Other);

        void operator+=(char const& Other);

        void operator=(String const& Other);

        void operator+=(String const& Other);

        void operator=(const char* Other);

        void operator+=(const char* Other);

        char& operator[](uint32_t Index); 

        const char& operator[](uint32_t Index) const; 

        String(const char* Other);

        String() = default;

        ~String();

    private:
    
        char* Data = nullptr;

        uint32_t Size = 0;

        uint32_t ReservedSize = 0;
    };
}