#pragma once

#include <stdint.h>

#include "STL/System/System.h"

namespace STL
{
    template<typename T, int MaxSize>
    class Array
    {
    public:

        void Push(T const& NewElement)
        {      
            if (this->Size < MaxSize)
            {
                Data[Size] = NewElement;
                Size++;
            }
        }

        T Pop()
        {      
            this->Size--;
            return this->Data[Size];
        }

        uint32_t Length() const
        {
            return this->Size;
        }   

        void Clear()
        {
            this->Size = 0;
        }

        void Erase(uint32_t Index)
        {
            for (uint32_t i = Index + 1; i < this->Size; i++)
            {
                this->Data[i - 1] = this->Data[i];
            }

            this->Size--;
        }

        int64_t Find(T Target)
        {
            for (uint32_t i = 0; i < this->Size; i++)
            {
                if (this->Data[i] == Target)
                {
                    return i;
                }
            }

            return -1;
        }

        T& Last()
        {            
            return Data[this->Size - 1];
        }

        T& operator[](uint32_t Index)
        {
            return Data[Index];
        }

        Array()
        {
            this->Size = 0;
        }

        ~Array()
        {
            this->Clear();
        }

    private:

        T Data[MaxSize];

        uint32_t Size = 0;
    };  
}