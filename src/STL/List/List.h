#pragma once

#include <stdint.h>

#include "STL/System/System.h"

namespace STL
{
    template<typename T>
    class List
    {
    public:

        void Reserve(uint32_t MinSize)
        {
            if (MinSize <= this->ReservedSize)
            {
                return;
            }

            this->ReservedSize = MinSize * 2;    
            T* NewData = (T*)Malloc(this->ReservedSize);

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

        void Push(T const& NewElement)
        {      
            this->Size++;
            this->Reserve(this->Size);
            this->Data[Size - 1] = NewElement;
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
            Free(this->Data);
            this->Data = nullptr;
            this->Size = 0;
            this->ReservedSize = 0;
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

        ~List()
        {
            this->Clear();
        }

    private:

        T* Data = nullptr;

        uint32_t Size = 0;
        uint32_t ReservedSize = 0;
    };  
}