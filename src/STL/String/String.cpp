#include "String.h"
#include "cstr.h"

#include "STL/System/System.h"
#include "STL/Memory/Memory.h"
#include "STL/Math/Math.h"

namespace STL
{        
    char String::Pop()
    {
        this->Size--;
        char Temp = this->Data[this->Size];
        return Temp;
    }

    char* String::cstr() const
    {
        this->Data[this->Size] = 0;
        return this->Data;
    }

    uint32_t String::Length() const
    {
        return this->Size;
    }

    void String::Erase(uint32_t Index, uint32_t Amount)
    {
        for (uint32_t i = Index + Amount; i < this->Size; i++)
        {
            this->Data[i - Amount] = this->Data[i];
        }

        this->Size--;
    }

    void String::Reserve(uint32_t MinSize)
    {
        if (MinSize <= this->ReservedSize)
        {
            return;
        }

        this->ReservedSize = MinSize * 2;    
        char* NewData = (char*)Malloc(ReservedSize);

        if (this->Data != nullptr)
        {        
            for (uint32_t i = 0; i < this->Size; i++)
            {
                NewData[i] = this->Data[i];
            }
            Free(this->Data);
        }

        this->Data = NewData;
    }

    bool String::operator==(const char* Other)
    {
        uint32_t OtherLength = STL::Length(Other);

        if (this->Size == OtherLength)
        {
            for (uint32_t i = 0; i < this->Size; i++)
            {
                if (this->Data[i] != Other[i])
                {
                    return false;
                }
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    bool String::operator==(String& Other)
    {
        return (*this == Other.cstr());
    }

    void String::operator+=(char const& Other)
    {
        this->Size++;
        this->Reserve(this->Size + 1);

        this->Data[this->Size - 1] = Other;
    }

    void String::operator=(char const& Other)
    {
        this->Reserve(2);

        this->Data[0] = Other;
        this->Size = 1;
    }

    void String::operator=(String const& Other)
    {
        this->operator=(Other.cstr());
    }

    void String::operator+=(String const& Other)
    {
        this->operator+=(Other.cstr());
    }

    void String::operator=(const char* Other)
    {       
        uint32_t OtherLength = STL::Length(Other);
        this->Reserve(OtherLength + 1);

        for (uint32_t i = 0; i < OtherLength; i++)
        {
            this->Data[i] = Other[i];
        }
        this->Size = OtherLength;
    }

    void String::operator+=(const char* Other)
    {
        uint32_t OtherLength = STL::Length(Other);
        this->Reserve(this->Size + OtherLength + 1);

        for (uint32_t i = 0; i < OtherLength; i++)
        {
            this->Data[this->Size + i] = Other[i];
        }
        this->Size += OtherLength;
    }

    char& String::operator[](uint32_t Index)
    {
        return this->Data[Index];
    }

    const char& String::operator[](uint32_t Index) const
    {
        return this->Data[Index];
    }

    String::String(const char* Other)
    {
        this->operator=(Other);
    }

    String::~String()
    {
        if (this->Data != nullptr)
        {
            Free(this->Data);
        }
    }
}