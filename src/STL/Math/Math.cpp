#include "Math.h"

namespace STL
{
    uint32_t GetDigit(uint32_t Number, uint32_t Digit)
    {
        uint32_t Y = IntPow(10, Digit);
        uint32_t Z = Number / Y;
        uint32_t X2 = Z / 10;
        return Z - X2 * 10;
    }

    uint32_t GetDigitAmount(uint32_t Number)
    {
        uint32_t Digits = 0;
        while (Number != 0)
        {
            Number /= 10;
            Digits++;
        }

        return Digits;
    }

    uint32_t IntPow(uint32_t X, uint32_t E)
    {
        uint32_t Y = 1;

        for (uint32_t i = 0; i < E; i++)
        {
            Y *= X;
        }

        return Y;
    }

    double Sqrt(double X)
    {
        float i = 0;
        float X1, X2;
        while(i * i <= X)
        {
            i += 0.1f;
        }
        X1=i;

        for (int j = 0; j < 10; j++)
        {
            X2 = X;
            X2 /= X1;
            X2 += X1;
            X2 /= 2;
            X1 = X2;
        }

        return X2;
    }
}
